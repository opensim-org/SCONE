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

namespace scone
{
	NeuralController::NeuralController( const PropNode& pn, Params& par, Model& model, const Locality& locality ) :
	Controller( pn, par, model, locality ),
	model_( model )
	{
		if ( pn.has_key( "delay_file" ) )
			delays_ = load_prop( scone::GetFolder( SCONE_SCENARIO_FOLDER ) / pn.get< path >( "delay_file" ) );

		activation_function = GetActivationFunction( pn.get< string >( "activation_function", "rectifier" ) );

		// automatic neural network
		if ( auto* sensors = pn.try_get_child( "SensorNeurons" ) )
		{
			for ( auto& n : pn.get_child( "SensorNeurons" ) )
				AddSensorNeurons( n.second, par );
		}

		if ( auto* n = pn.try_get_child( "PatternNeurons" ) )
			AddPatternNeurons( *n, par );

		if ( auto* n = pn.try_get_child( "InterNeurons" ) )
		{
			AddInterNeuronLayer();
			AddInterNeurons( *n, par, false );
			AddInterNeurons( *n, par, true );
		}

		if ( auto* n = pn.try_get_child( "MotorNeurons" ) )
			AddMotorNeurons( *n, par, false );
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

	void NeuralController::AddInterNeuronLayer()
	{
		m_InterNeurons.emplace_back();
	}

	void NeuralController::AddInterNeurons( const PropNode& pn, Params& par, bool mirrored )
	{
		// Must call AddInterNeuronLayer before!
		SCONE_ASSERT( m_InterNeurons.size() >= 1 );
		Index prev_layer = m_InterNeurons.size() - 1;

		int amount = pn.get< int >( "amount" );
		for ( int i = 0; i < amount; ++i )
		{
			auto name = stringf( "N%d", i ) + ( mirrored ? "_r" : "_l" );
			ScopedParamSetPrefixer ps( par, GetNameNoSide( name ) + "." );

			auto offset = par.try_get( "C0", pn, "offset", 0.0 );
			m_InterNeurons.back().emplace_back( std::make_unique< InterNeuron >( pn, par, *this, name ) );
			for ( Index idx = 0; idx < GetLayerSize( prev_layer ); ++idx )
			{
				auto s = GetNeuron( prev_layer, idx );
				if ( s->GetSide() == m_InterNeurons.back().back()->GetSide() )
				{
					auto w = par.try_get( s->GetParName() + ".w", pn, "weight", 1.0 );
					auto m = par.try_get( s->GetParName() + ".m", pn, "mean", 0.0 );
					m_InterNeurons.back().back()->AddInput( s, w, m );
				}
				//log::info( "added interneuron: ", name + "." + s->GetName( false ) );
			}
		}
	}

	void NeuralController::AddMotorNeurons( const PropNode& pn, Params& par, bool mirrored )
	{
		bool monosynaptic = pn.has_value( "monosynaptic" );
		bool antagonistic = pn.has_value( "antagonistic" );
		bool balance = pn.has_value( "balance" );
		bool pattern = pn.has_value( "pattern" );
		bool top_layer = pn.has_value( "top_layer" );

		auto muscle_names = FindMatchingNames( GetModel().GetMuscles(), pn.get< string >( "include", "*" ), pn.get< string >( "exclude", "" ) );
		for ( auto& name : muscle_names )
		{
			ScopedParamSetPrefixer ps( par, GetNameNoSide( name ) + "." );
			Side muscle_side = GetSideFromName( name );
			bool right_muscle = muscle_side == RightSide;

			// create the motor neuron
			m_MotorNeurons.emplace_back( std::make_unique< MotorNeuron >( pn, par, *this, name ) );

			// add inputs
			if ( top_layer )
			{
				for ( Index idx = 0; idx < m_InterNeurons.back().size(); ++idx )
				{
					auto input = m_InterNeurons.back()[ idx ].get();
					auto gain = par.get( input->GetName( right_muscle ), pn.get_child( "top_layer" ) );
					m_MotorNeurons.back()->AddInput( input, gain );
				}
			}

			if ( monosynaptic || antagonistic || balance )
			{
				for ( Index idx = 0; idx < m_SensorNeurons.size(); ++idx )
				{
					auto input = m_SensorNeurons[ idx ].get();
					if ( monosynaptic && input->source_name_ == name )
						m_MotorNeurons.back()->AddInput( input, par.get( input->type_, pn[ "monosynaptic" ] ) );

					if ( antagonistic )
					{
						auto it1 = TryFindByName( GetModel().GetMuscles(), name );
						auto it2 = TryFindByName( GetModel().GetMuscles(), input->source_name_ );
						if ( it1 != GetModel().GetMuscles().end() && it2 != GetModel().GetMuscles().end() )
						{
							if ( (*it1)->IsAntagonist( **it2 ) )
								m_MotorNeurons.back()->AddInput( input, par.get( GetNameNoSide( input->source_name_ ) + "." + input->type_, pn[ "antagonistic" ] ) );
						}
					}

					if ( balance && ( input->type_ == "DP" || input->type_ == "DV" ) )
						m_MotorNeurons.back()->AddInput( input, par.get( input->type_, pn[ "balance" ] ) );
				}
			}

			if ( pattern )
			{
				for ( auto& n : m_PatternNeurons )
					if ( n->mirrored_ == right_muscle )
						m_MotorNeurons.back()->AddInput( n.get(), par.get( n->name_, pn[ "pattern" ] ) );
			}

			m_MotorNeurons.back()->offset_ = par.try_get( "C0", pn, "offset", 0.0 );
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
