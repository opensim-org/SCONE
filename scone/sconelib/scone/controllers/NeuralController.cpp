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

namespace scone
{
	NeuralController::NeuralController( const PropNode& pn, Params& par, Model& model, const Locality& locality ) :
	Controller( pn, par, model, locality ),
	model_( model )
	{
		if ( pn.has_key( "delay_file" ) )
			delays_ = load_prop( scone::GetFolder( SCONE_SCENARIO_FOLDER ) / pn.get< path >( "delay_file" ) );

		activation_function = GetActivationFunction( pn.get< string >( "activation", "rectifier" ) );

		// create sensor neurons
		for ( auto& n : pn.get_child( "SensorNeuronLayer" ) )
			AddSensorNeurons( n.second, par );

		// create inter neurons
		for ( auto& n : pn.select( "InterNeuronLayer" ) )
			AddInterNeuronLayer( n.second, par );

		// create sensor neurons
		for ( auto& n : pn.get_child( "MotorNeuronLayer" ) )
			AddMotorNeurons( n.second, par );
	}

	void NeuralController::AddSensorNeurons( const PropNode& pn, Params& par )
	{
		auto type = pn.get< string >( "type" );
		std::vector< string > sources;
		if ( type == "L" || type == "F" || type == "S" )
			sources = FindMatchingNames( GetModel().GetMuscles(), pn.get< string >( "source" ), pn.get< string >( "exclude", "" ) );
		else if ( type == "DP" || type == "DV" )
			sources = FindMatchingNames( GetModel().GetDofs(), pn.get< string >( "source" ), pn.get< string >( "exclude", "" ) );

		for ( auto& name : sources )
			m_SensorNeurons.emplace_back( std::make_unique< SensorNeuron >( pn, par, *this, name, "linear" ) );
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
		Index layer = pn.get< Index >( "layer", 1 );
		int amount = pn.get< int >( "neurons" );
		string act_func = pn.get< string >( "activation", "rectifier" );

		// allocate layer
		SCONE_ASSERT( layer > 0 && layer == m_InterNeurons.size() + 1 );
		m_InterNeurons.resize( std::max( layer, m_InterNeurons.size() ) );

		for ( int i = 0; i < amount; ++i )
		{
			for ( bool mirrored : { false, true } )
			{
				auto name = stringf( "N%d_%d", layer, i ) + ( mirrored ? "_r" : "_l" );
				ScopedParamSetPrefixer ps( par, GetNameNoSide( name ) + "." );
				m_InterNeurons[ layer - 1 ].emplace_back( std::make_unique< InterNeuron >( pn, par, name, act_func ) );

				for ( auto& child : pn.select( "InterNeuron" ) )
				{
					SCONE_ASSERT( child.first == "InterNeuron" );
					m_InterNeurons[ layer - 1 ].back()->AddInputs( child.second, par, *this );
				}
			}
		}
	}

	void NeuralController::AddMotorNeurons( const PropNode& pn, Params& par )
	{
		string input_type = pn.get< string >( "type", "*" );
		bool monosynaptic = pn.get( "monosynaptic", false );
		bool antagonistic = pn.get( "antagonistic", false );
		Index input_layer = pn.get< Index >( "input_layer" );

		auto muscle_names = FindMatchingNames( GetModel().GetMuscles(), pn.get< string >( "include", "*" ), pn.get< string >( "exclude", "" ) );
		for ( auto& name : muscle_names )
		{
			ScopedParamSetPrefixer ps( par, GetNameNoSide( name ) + "." );

			// create the motor neuron
			auto it = flut::find_if( m_MotorNeurons, [&]( MotorNeuronUP& m ) { return m->name_ == name; } );
			if ( it == m_MotorNeurons.end() )
			{
				m_MotorNeurons.emplace_back( std::make_unique< MotorNeuron >( pn, par, *this, name ) );
				it = m_MotorNeurons.end() - 1;
			}

			(*it)->AddInputs( pn, par, *this );
			(*it)->offset_ = par.try_get( "C0", pn, "offset", 0.0 );
		}
	}

	scone::Controller::UpdateResult NeuralController::UpdateControls( Model& model, double timestamp )
	{
		for ( auto& n : m_MotorNeurons )
			n->UpdateActuator();

		return Controller::SuccessfulUpdate;
	}

	Neuron* NeuralController::FindInput( const PropNode& pn, Locality loc )
	{
		if ( pn.get_any< bool >( { "mirrored", "opposite" }, false ) )
			loc = MakeMirrored( loc );
		auto name = loc.ConvertName( pn.get< string >( "source", "leg0" ) );

		if ( pn.get< string >( "type" ) != "Neuron" )
			name += '.' + pn.get< string >( "type" );

		auto iter = flut::find_if( m_InterNeurons.back(), [&]( const InterNeuronUP& n ) { return n->GetName( loc.mirrored ) == name; } );
		return iter != m_InterNeurons.back().end() ? iter->get() : nullptr;
	}

	void NeuralController::StoreData( Storage<Real>::Frame& frame, const StoreDataFlags& flags ) const
	{
		for ( auto& neuron : m_PatternNeurons )
			frame[ "neuron." + neuron->GetName( false ) ] = neuron->output_;
		for ( auto& neuron : m_SensorNeurons )
			frame[ "neuron." + neuron->GetName( false ) ] = neuron->output_;
		for ( auto& layer : m_InterNeurons )
			for ( auto& neuron : layer )
				frame[ "neuron." + neuron->GetName( false ) ] = neuron->output_;
	}

	void NeuralController::WriteResult( const path& file ) const
	{
		//std::ofstream str( ( file + ".neural_weights.txt" ).str() );
		flut::table< double > data;

		for ( auto& inter_layer : m_InterNeurons )
		{
			for ( auto& neuron : inter_layer )
			{
				for ( auto& input : neuron->inputs_ )
					data( neuron->name_, input.neuron->GetName( false ) ) = input.weight;
			}
		}

		for ( auto& neuron : m_MotorNeurons )
		{
			for ( auto& input : neuron->inputs_ )
				data( neuron->name_, input.neuron->GetName( false ) ) = input.weight;
		}
		std::ofstream( ( file + ".neural_weights.txt" ).str() ) << data;
	}

	String NeuralController::GetClassSignature() const
	{
		size_t c = 0;
		for ( auto& layer : m_InterNeurons )
			for ( auto& neuron : layer )
				c += neuron->GetInputCount();

		for ( auto& neuron : m_MotorNeurons )
			c += neuron->GetInputCount();

		return flut::stringf( "N%d", c );
	}
}
