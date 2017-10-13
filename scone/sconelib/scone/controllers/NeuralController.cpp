#include "NeuralController.h"

#include "scone/core/string_tools.h"
#include "scone/model/Locality.h"
#include "scone/core/HasName.h"
#include "flut/container_tools.hpp"
#include "SensorNeuron.h"
#include "InterNeuron.h"
#include "PatternNeuron.h"
#include "flut/pattern_matcher.hpp"
#include "../model/Model.h"
#include "../model/Muscle.h"
#include "../model/Dof.h"
#include <algorithm>
#include "activation_functions.h"
#include "flut/hash.hpp"
#include "flut/string_tools.hpp"
#include "flut/table.hpp"
#include "flut/system/string_cast.hpp"

namespace scone
{
	NeuralController::NeuralController( const PropNode& pn, Params& par, Model& model, const Locality& locality ) :
	Controller( pn, par, model, locality ),
	model_( model )
	{
		if ( pn.has_key( "delay_file" ) )
			delays_ = load_prop( scone::GetFolder( SCONE_SCENARIO_FOLDER ) / pn.get< path >( "delay_file" ) );

		activation_function = GetActivationFunction( pn.get< string >( "activation", "rectifier" ) );

		// create sensor neuron layer
		AddSensorNeuronLayer( pn.get_child( "SensorNeuronLayer" ), par );

		// create inter neuron layers
		for ( auto& n : pn.select( "InterNeuronLayer" ) )
			AddInterNeuronLayer( n.second, par );

		// create motor neuron layer
		AddMotorNeuronLayer( pn.get_child( "MotorNeuronLayer" ), par );
	}

	void NeuralController::AddSensorNeuronLayer( const PropNode& layer_pn, Params& par )
	{
		for ( auto& child_kvp : layer_pn )
		{
			auto& child_pn = child_kvp.second;

			auto type = child_pn.get< string >( "type" );
			auto source_mask = child_pn.get< string >( "source" );
			auto exclude_mask = child_pn.get< string >( "exclude", "" );
			std::vector< string > source_names;

			if ( type == "L" || type == "F" || type == "S" )
				source_names = FindMatchingNames( GetModel().GetMuscles(), source_mask, exclude_mask );
			else if ( type == "DP" || type == "DV" )
				source_names = FindMatchingNames( GetModel().GetDofs(), source_mask, exclude_mask );

			for ( auto& name : source_names )
				m_SensorNeurons.emplace_back( std::make_unique< SensorNeuron >( child_pn, par, *this, name, m_SensorNeurons.size(), GetSideFromName( name ), "linear" ) );
		}
	}

	void NeuralController::AddPatternNeurons( const PropNode& pn, Params& par )
	{
		auto amount = pn.get< int >( "amount" );
		for ( int i = 0; i < amount; ++i )
			m_PatternNeurons.emplace_back( std::make_unique< PatternNeuron >( pn, par, *this, i, false ) );
		for ( int i = 0; i < amount; ++i )
			m_PatternNeurons.emplace_back( std::make_unique< PatternNeuron >( pn, par, *this, i, true ) );
	}

	void NeuralController::AddInterNeuronLayer( const PropNode& pn, Params& par )
	{
		auto layer_name = FixLayerName( pn.get< string >( "layer" ) );
		int amount = pn.get< int >( "neurons" );
		string act_func = pn.get< string >( "activation", "rectifier" );

		auto& layer = m_InterNeurons[ layer_name ];
		for ( int i = 0; i < amount; ++i )
		{
			for ( auto side : { LeftSide, RightSide } )
			{
				ScopedParamSetPrefixer ps( par, layer_name + stringf( "_%d.", i ) );
				layer.emplace_back( std::make_unique< InterNeuron >( pn, par, layer_name, i, side, act_func ) );
				for ( auto& child : pn )
					layer.back()->AddInputs( child.second, par, *this );
			}
		}
	}

	void NeuralController::AddMotorNeuronLayer( const PropNode& pn, Params& par )
	{
		for ( auto& muscle : GetModel().GetMuscles() )
		{
			auto name = muscle->GetName();
			ScopedParamSetPrefixer ps( par, GetNameNoSide( name ) + "." );
			m_MotorNeurons.emplace_back( std::make_unique< MotorNeuron >( pn, par, *this, name, m_MotorNeurons.size(), GetSideFromName( name ) ) );
			for ( auto& child_pn : pn )
			{
				auto include = child_pn.second.get< flut::pattern_matcher >( "include", "*" );
				auto exclude = child_pn.second.get< flut::pattern_matcher >( "exclude", "" );
				if ( include( name ) && !exclude( name ) )
					m_MotorNeurons.back()->AddInputs( child_pn.second, par, *this );
			}
		}
	}

	scone::Controller::UpdateResult NeuralController::UpdateControls( Model& model, double timestamp )
	{
		for ( auto& n : m_MotorNeurons )
			n->UpdateActuator();

		return Controller::SuccessfulUpdate;
	}

	void NeuralController::StoreData( Storage<Real>::Frame& frame, const StoreDataFlags& flags ) const
	{
		for ( auto& neuron : m_PatternNeurons )
			frame[ "neuron." + neuron->GetName( false ) ] = neuron->output_;
		for ( auto& neuron : m_SensorNeurons )
			frame[ "neuron." + neuron->GetName( false ) ] = neuron->output_;
		for ( auto& layer : m_InterNeurons )
			for ( auto& neuron : layer.second )
				frame[ "neuron." + neuron->GetName( false ) ] = neuron->output_;
	}

	void NeuralController::WriteResult( const path& file ) const
	{
		flut::table< double > weights, contribs;

		for ( auto& inter_layer : m_InterNeurons )
		{
			for ( auto& neuron : inter_layer.second )
			{
				for ( auto& input : neuron->inputs_ )
					weights( input.neuron->GetName( false ), neuron->name_ ) = input.gain;
			}
		}

		for ( auto& neuron : m_MotorNeurons )
		{
			auto tot = std::accumulate( neuron->inputs_.begin(), neuron->inputs_.end(), 0.0, [&]( double v1, const InterNeuron::Input& i ) { return v1 + i.contribution; } );
			for ( auto& input : neuron->inputs_ )
			{
				weights( input.neuron->GetName( false ), neuron->name_ ) = input.gain;
				contribs( input.neuron->GetName( false ), neuron->name_ ) = input.contribution / tot;
			}
		}
		std::ofstream( ( file + ".stats.weights.txt" ).str() ) << weights;
		std::ofstream( ( file + ".stats.contrib.txt" ).str() ) << contribs;
	}

	String NeuralController::GetClassSignature() const
	{
		size_t c = 0;
		for ( auto& layer : m_InterNeurons )
			for ( auto& neuron : layer.second )
				c += neuron->GetInputCount();

		for ( auto& neuron : m_MotorNeurons )
			c += neuron->GetInputCount();

		return flut::stringf( "N%d", c );
	}
}
