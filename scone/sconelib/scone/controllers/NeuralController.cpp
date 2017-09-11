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
		INIT_PROP( pn, mean_, 0.0 );
		INIT_PROP( pn, std_, 0.1 );
		activation_function = GetActivationFunction( pn.get< string >( "activation_function" ) );

		if ( auto neurons = pn.try_get_child( "Neurons" ) )
		{
			// hand-designed neural network
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
				AddInterNeurons( *n, par, model, Locality( NoSide, false ) );
				AddInterNeurons( *n, par, model, Locality( NoSide, true ) );
			}

			if ( auto* n = pn.try_get_child( "MotorNeurons" ) )
				AddMotorNeurons( *n, par, model, locality );
		}
	}

	scone::InterNeuron* NeuralController::AddInterNeuron( const PropNode& pn, Params& par, Model& model, Locality loc )
	{
		m_Neurons.resize( std::max( (int)m_Neurons.size(), 2 ) );
		m_Neurons.back().emplace_back( std::make_unique< InterNeuron >( pn, par, model, *this, loc ) );
		return dynamic_cast<InterNeuron*>( m_Neurons.back().back().get() );
	}

	SensorNeuron* NeuralController::AddSensorNeuron( const PropNode& pn, Params& par, Model& model, Locality loc )
	{
		m_Neurons.resize( std::max( (int)m_Neurons.size(), 1 ) );
		m_Neurons.front().push_back( std::make_unique< SensorNeuron >( *this, pn, par, model, loc ) );
		return dynamic_cast<SensorNeuron*>( m_Neurons.front().back().get() );
	}

	void NeuralController::AddSensorNeurons( const PropNode& pn, Params& par, Model& model, Locality loc )
	{
		m_Neurons.resize( std::max( (int)m_Neurons.size(), 1 ) );

		auto type = pn.get< string >( "type" );
		std::vector< string > sources;
		if ( type == "L" || type == "F" )
			sources = FindMatchingNames( model.GetMuscles(), pn.get< string >( "include" ), pn.get< string >( "exclude", "" ) );
		else if ( type == "DP" || type == "DV" )
			sources = FindMatchingNames( model.GetDofs(), pn.get< string >( "include" ), pn.get< string >( "exclude", "" ) );

		double delay = 0.01;
		double offset = 0.0;
		for ( auto& name : sources )
			m_Neurons.front().emplace_back( std::make_unique< SensorNeuron >( *this, model, type, name, delay, offset ) );
	}

	void NeuralController::AddInterNeurons( const PropNode& pn, Params& par, Model& model, Locality loc )
	{
		SCONE_ASSERT( m_Neurons.size() > 0 );
		Index prev_layer = m_Neurons.size() - 1;
		m_Neurons.resize( m_Neurons.size() + 1 );
		int amount = pn.get< int >( "amount" );
		for ( int i = 0; i < amount; ++i )
		{
			auto neuron = std::make_unique< InterNeuron >( *this, stringf( "I%d", i ) + ( loc.mirrored ? "_r" : "_l" ) );
			ScopedParamSetPrefixer ps( par, stringf( "N%d.", i ) );
			for ( auto& s : m_Neurons[ prev_layer ] )
			{
				auto w = par.get( s->GetName( loc.mirrored ), mean_, std_);
				neuron->AddInput( w, s.get() );
			}
			m_Neurons.back().push_back( std::move( neuron ) );
		}
	}

	void NeuralController::AddMotorNeurons( const PropNode& pn, Params& par, Model& model, Locality loc )
	{
		SCONE_ASSERT( m_Neurons.size() > 0 );
		auto muscle_names = FindMatchingNames( model.GetMuscles(), pn.get< string >( "include", "*" ), pn.get< string >( "exclude", "" ) );
		for ( auto& name : muscle_names )
		{
			auto* muscle = FindByName( model.GetMuscles(), name ).get();
			auto neuron = std::make_unique< MotorNeuron >( *this, muscle, name );
			loc.mirrored = GetSideFromName( name ) == RightSide;
			ScopedParamSetPrefixer ps( par, GetNameNoSide( name ) + "." );
			for ( Index idx = 0; idx < m_Neurons.back().size(); ++idx )
			{
				auto input = m_Neurons.back()[ idx ].get();
				auto w = par.get( input->GetName( loc.mirrored ), mean_, std_ );
				neuron->AddInput( w, input );
			}
			m_MotorNeurons.push_back( std::move( neuron ) );
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

		auto iter = flut::find_if( m_Neurons.back(), [&]( const NeuronUP& n ) { return n->GetName( loc.mirrored ) == name; } );
		return iter != m_Neurons.back().end() ? iter->get() : nullptr;
	}

	void NeuralController::StoreData( Storage<Real>::Frame& frame, const StoreDataFlags& flags )
	{
		for ( auto& layer : m_Neurons )
			for ( auto& neuron : layer )
				frame[ "neuron." + neuron->GetName( false ) ] = neuron->output_;
	}

	String NeuralController::GetClassSignature() const
	{
		size_t c = 0;
		for ( auto& layer : m_Neurons )
			for ( auto& neuron : layer )
				c += neuron->GetInputCount();

		for ( auto& neuron : m_MotorNeurons )
			c += neuron->GetInputCount();

		return flut::stringf( "N%d", c );
	}
}
