#include "NeuralController.h"

#include "scone/core/string_tools.h"
#include "scone/model/Locality.h"
#include "scone/core/HasName.h"
#include "flut/container_tools.hpp"
#include "SensorNeuron.h"
#include "InterNeuron.h"
#include "flut/pattern_matcher.hpp"
#include "../model/Model.h"
#include "../model/Muscle.h"
#include "../model/Dof.h"
#include <algorithm>
#include "activation_functions.h"

namespace scone
{
	NeuralController::NeuralController( const PropNode& pn, Params& par, Model& model, const Locality& locality ) :
	Controller( pn, par, model, locality )
	{
		INIT_PROP( pn, std_, 0.1 );

		if ( pn.has_key( "delay_file" ) )
			delays_ = load_prop( scone::GetFolder( SCONE_SCENARIO_FOLDER ) / pn.get< path >( "delay_file" ) );

		activation_function = GetActivationFunction( pn.get< string >( "activation_function", "rectifier" ) );

		if ( auto neurons = pn.try_get_child( "Neurons" ) )
		{
			// hand-designed neural network
			AddInterNeuronLayer();
			for ( auto& neuron : *neurons )
			{
				AddInterNeuron( neuron.second, par, model, Locality( LeftSide ) );
				AddInterNeuron( neuron.second, par, model, Locality( RightSide ) );
			}
		}
		else
		{
			// automatic neural network
			if ( auto* sensors = pn.try_get_child( "SensorNeurons" ) )
			{
				for ( auto& n : pn.get_child( "SensorNeurons" ) )
					AddSensorNeurons( n.second, par, model, locality );
			}

			if ( auto* n = pn.try_get_child( "InterNeurons" ) )
			{
				AddInterNeuronLayer();
				AddInterNeurons( *n, par, model, Locality( NoSide, false ) );
				AddInterNeurons( *n, par, model, Locality( NoSide, true ) );
			}

			if ( auto* n = pn.try_get_child( "MotorNeurons" ) )
				AddMotorNeurons( *n, par, model, locality );
		}
	}

	scone::InterNeuron* NeuralController::AddInterNeuron( const PropNode& pn, Params& par, Model& model, Locality loc )
	{
		m_InterNeurons.resize( std::max( (int)m_InterNeurons.size(), 2 ) );
		m_InterNeurons.back().emplace_back( std::make_unique< InterNeuron >( pn, par, model, *this, loc ) );
		return m_InterNeurons.back().back().get();
	}

	SensorNeuron* NeuralController::AddSensorNeuron( const PropNode& pn, Params& par, Model& model, Locality loc )
	{
		m_SensorNeurons.push_back( std::make_unique< SensorNeuron >( *this, pn, par, model, loc ) );
		return m_SensorNeurons.back().get();
	}

	void NeuralController::AddSensorNeurons( const PropNode& pn, Params& par, Model& model, Locality loc )
	{
		auto type = pn.get< string >( "type" );
		std::vector< string > sources;
		if ( type == "L" || type == "F" )
			sources = FindMatchingNames( model.GetMuscles(), pn.get< string >( "include" ), pn.get< string >( "exclude", "" ) );
		else if ( type == "DP" || type == "DV" )
			sources = FindMatchingNames( model.GetDofs(), pn.get< string >( "include" ), pn.get< string >( "exclude", "" ) );

		for ( auto& name : sources )
		{
			double offset = type == "L" ? 1.0 : 0.0;
			double delay = delays_.get< double >( GetNameNoSide( name ) );
			m_SensorNeurons.emplace_back( std::make_unique< SensorNeuron >( *this, model, type, name, delay, offset ) );
		}
	}

	void NeuralController::AddInterNeuronLayer()
	{
		m_InterNeurons.emplace_back();
	}

	void NeuralController::AddInterNeurons( const PropNode& pn, Params& par, Model& model, Locality loc )
	{
		// Must call AddInterNeuronLayer before!
		SCONE_ASSERT( m_InterNeurons.size() >= 1 );
		Index prev_layer = m_InterNeurons.size() - 1;

		int amount = pn.get< int >( "amount" );
		for ( int i = 0; i < amount; ++i )
		{
			auto name = stringf( "N%d", i ) + ( loc.mirrored ? "_r" : "_l" );
			ScopedParamSetPrefixer ps( par, GetNameNoSide( name ) + "." );

			auto offset = par.get( "C0", 0.0, std_ );
			m_InterNeurons.back().emplace_back( std::make_unique< InterNeuron >( *this, name ) );

			for ( Index idx = 0; idx < GetLayerSize( prev_layer ); ++idx )
			{
				auto s = GetNeuron( prev_layer, idx );
				auto w = par.get( s->GetName( loc.mirrored ), 0.0, std_);
				m_InterNeurons.back().back()->AddInput( w, s );
				//log::info( "added ", s->GetName( loc.mirrored ) );
			}
		}
	}

	void NeuralController::AddMotorNeurons( const PropNode& pn, Params& par, Model& model, Locality loc )
	{
		bool monosynaptic = pn.get< bool >( "monosynaptic", false );
		bool antagonistic = pn.get< bool >( "antagonistic", false );
		bool balance = pn.get< bool >( "balance", false );

		auto muscle_names = FindMatchingNames( model.GetMuscles(), pn.get< string >( "include", "*" ), pn.get< string >( "exclude", "" ) );
		for ( auto& name : muscle_names )
		{
			auto* muscle = FindByName( model.GetMuscles(), name ).get();
			m_MotorNeurons.emplace_back( std::make_unique< MotorNeuron >( *this, muscle, name ) );
			loc.mirrored = GetSideFromName( name ) == RightSide;

			ScopedParamSetPrefixer ps( par, GetNameNoSide( name ) + "." );

			if ( pn.get< bool >( "top_layer", true ) )
			{
				for ( Index idx = 0; idx < m_InterNeurons.back().size(); ++idx )
				{
					auto input = m_InterNeurons.back()[ idx ].get();
					auto weight = par.get( input->GetName( loc.mirrored ), 0.0, std_ );
					m_MotorNeurons.back()->AddInput( weight, input );
				}
			}

			if ( monosynaptic || antagonistic )
			{
				for ( Index idx = 0; idx < m_SensorNeurons.size(); ++idx )
				{
					auto input = m_SensorNeurons[ idx ].get();
					if ( monosynaptic && input->source_name_ == name )
						m_MotorNeurons.back()->AddInput( par.get( input->type_, 0.0, std_ ), input );

					if ( antagonistic )
					{
						auto it1 = TryFindByName( model.GetMuscles(), name );
						auto it2 = TryFindByName( model.GetMuscles(), input->source_name_ );
						if ( it1 != model.GetMuscles().end() && it2 != model.GetMuscles().end() )
						{
							if ( (*it1)->IsAntagonist( **it2 ) )
								m_MotorNeurons.back()->AddInput( par.get( GetNameNoSide( input->source_name_ ) + "." + input->type_, 0.0, std_ ), input );
						}
					}

					if ( balance && ( input->type_ == "DP" || input->type_ == "DV" ) )
						m_MotorNeurons.back()->AddInput( par.get( input->type_, 0.0, std_ ), input );
				}
			}
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
					str << "\t" << input.second->GetName( false ) << "\t" << input.first;
				str << std::endl;
			}
		}

		for ( auto& neuron : m_MotorNeurons )
		{
			str << neuron->name_ << "\t" << neuron->offset_;
			for ( auto& input : neuron->inputs_ )
				str << "\t" << input.second->GetName( false ) << "\t" << input.first;
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
