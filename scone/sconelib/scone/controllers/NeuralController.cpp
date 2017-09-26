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

namespace scone
{
	NeuralController::NeuralController( const PropNode& pn, Params& par, Model& model, const Locality& locality ) :
	Controller( pn, par, model, locality ),
	model_( model )
	{
		if ( pn.has_key( "delay_file" ) )
			delays_ = load_prop( scone::GetFolder( SCONE_SCENARIO_FOLDER ) / pn.get< path >( "delay_file" ) );

		activation_function = GetActivationFunction( pn.get< string >( "activation", "rectifier" ) );

		// automatic neural network
		if ( auto* neurons = pn.try_get_child( "Neurons" ) )
		{
			for ( auto& n : *neurons )
			{
				switch ( flut::hash( n.first ) )
				{
				case "SensorNeuron"_hash: AddSensorNeurons( n.second, par ); break;
				case "PatternNeuron"_hash: AddPatternNeurons( n.second, par ); break;
				case "InterNeuron"_hash: AddInterNeurons( n.second, par ); break;
				case "MotorNeuron"_hash: AddMotorNeurons( n.second, par ); break;
				default: SCONE_THROW( "Unknown neuron type: " + n.first );
				}
			}
		}
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
		{
			m_SensorNeurons.emplace_back( std::make_unique< SensorNeuron >( pn, par, *this, name ) );
			//log::info( "added sensor", m_SensorNeurons.back()->GetName( false ) );
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

	void NeuralController::AddInterNeurons( const PropNode& pn, Params& par )
	{
		Index layer = pn.get< Index >( "layer", 1 );
		Index input_layer = pn.get< Index >( "input_layer", 0 );
		string input_type = pn.get< string >( "type", "*" );
		int amount = pn.get< int >( "amount" );

		// make sure layer exists
		SCONE_ASSERT( layer > 0 );
		m_InterNeurons.resize( std::max( layer, m_InterNeurons.size() ) );

		for ( bool mirrored : { false, true } )
		{
			for ( int i = 0; i < amount; ++i )
			{
				auto name = stringf( "N%d", i ) + ( mirrored ? "_r" : "_l" );
				ScopedParamSetPrefixer ps( par, GetNameNoSide( name ) + "." );
				auto& neuron_layer = m_InterNeurons[ layer - 1 ];

				auto it = flut::find_if( neuron_layer, [&]( InterNeuronUP& m ) { return m->name_ == name; } );
				if ( it == neuron_layer.end() )
				{
					neuron_layer.emplace_back( std::make_unique< InterNeuron >( pn, par, *this, name ) );
					it = neuron_layer.end() - 1;
				}

				(*it)->AddInputs( pn, par, *this );

				//for ( Index idx = 0; idx < GetLayerSize( input_layer ); ++idx )
				//{
				//	auto s = GetNeuron( input_layer, idx );
				//	if ( s->GetSide() == m_InterNeurons.back().back()->GetSide() )
				//	{
				//		auto input_name = s->GetParName();
				//		auto w = par.try_get( input_name + ".w", pn, "weight", 1.0 );
				//		auto m = par.try_get( input_name + ".m", pn, "mean", 0.0 );
				//		m_InterNeurons.back().back()->AddInput( s, w, m );
				//	}
				//	log::info( "added interneuron: ", name + "." + s->GetName( false ) );
				//}
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
		std::ofstream str( ( file + ".neural_weights.txt" ).str() );

		for ( Index i = 1; i < m_InterNeurons.size(); ++i )
		{
			for ( auto& neuron : m_InterNeurons[ i ] )
			{
				str << neuron->name_ << "\t" << neuron->offset_;
				for ( auto& input : neuron->inputs_ )
					str << "\t" << input.neuron->GetName( false ) << "\t" << input.weight;
				str << std::endl;
			}
		}

		for ( auto& neuron : m_MotorNeurons )
		{
			str << neuron->name_ << "\t" << neuron->offset_;
			for ( auto& input : neuron->inputs_ )
				str << "\t" << input.neuron->GetName( false ) << "\t" << input.weight;
			str << std::endl;
		}
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
