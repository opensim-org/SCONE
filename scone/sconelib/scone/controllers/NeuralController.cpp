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

namespace scone
{
	NeuralController::NeuralController( const PropNode& pn, Params& par, Model& model, const Locality& locality ) :
	Controller( pn, par, model, locality )
	{
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
		m_InterNeurons.emplace_back( std::make_unique< InterNeuron >( pn, par, model, *this, loc ) );
		return m_InterNeurons.back().get();
	}

	SensorNeuron* NeuralController::AddSensorNeuron( const PropNode& pn, Params& par, Model& model, Locality loc )
	{
		m_SensorNeurons.push_back( std::make_unique< SensorNeuron >( pn, par, model, loc ) );
		return m_SensorNeurons.back().get();
	}

	void NeuralController::AddMotorNeuron( Neuron* neuron, Actuator* act )
	{
		m_MotorNeurons.emplace_back( neuron, act );
	}

	void NeuralController::AddSensorNeurons( const PropNode& pn, Params& par, Model& model, Locality loc )
	{
		auto type = pn.get< string >( "type" );
		std::vector< string > sources;
		if ( type == "L" || type == "F" )
			sources = FindMatchingNames( model.GetMuscles(), pn.get< string >( "include" ), pn.get< string >( "exclude", "" ) );
		else if ( type == "DP" || type == "DV" )
			sources = FindMatchingNames( model.GetDofs(), pn.get< string >( "include" ), pn.get< string >( "exclude", "" ) );

		double delay = 0.01;
		double offset = 0.0;
		for ( auto& name : sources )
			m_SensorNeurons.emplace_back( std::make_unique< SensorNeuron >( model, loc, type, name, delay, offset ) );
	}

	void NeuralController::AddInterNeurons( const PropNode& pn, Params& par, Model& model, Locality loc )
	{
		int amount = pn.get< int >( "amount" );
		for ( int i = 0; i < amount; ++i )
		{
			auto neuron = std::make_unique< InterNeuron >( stringf( "I%d", i ) + ( loc.mirrored ? "_m" : "" ) );
			ScopedParamSetPrefixer ps( par, stringf( "N%d", i ) );

			for ( auto& s : m_SensorNeurons )
			{
				auto w = par.get( loc.ConvertName( s->name_ ) + ".W", 0.0, 0.1 );
				neuron->AddInput( w, s.get() );
			}
			m_InterNeurons.push_back( std::move( neuron ) );
		}
	}

	void NeuralController::AddMotorNeurons( const PropNode& pn, Params& par, Model& model, Locality loc )
	{
		size_t inter_count = m_InterNeurons.size();

		auto muscles = FindMatchingNames( model.GetMuscles(), pn.get< string >( "include", "*" ), pn.get< string >( "exclude", "" ) );
		for ( auto& name : muscles )
		{
			auto neuron = std::make_unique< InterNeuron >( name );
			ScopedParamSetPrefixer ps( par, name );
			for ( Index idx = 0; idx < inter_count; ++idx )
			{
				auto w = par.get( loc.ConvertName( m_InterNeurons[ idx ]->name_ ) + ".W", 0.0, 0.1 );
				neuron->AddInput( w, m_InterNeurons[ idx ].get() );
			}
			m_InterNeurons.push_back( std::move( neuron ) );
			m_MotorNeurons.emplace_back( m_InterNeurons.back().get(), FindByName( model.GetMuscles(), name ).get() );
		}
	}

	scone::Controller::UpdateResult NeuralController::UpdateControls( Model& model, double timestamp )
	{
		for ( auto& n : m_MotorNeurons )
			n.UpdateActuator();

		return Controller::SuccessfulUpdate;
	}

	Neuron* NeuralController::FindInput( const PropNode& pn, Locality loc )
	{
		if ( pn.get_any< bool >( { "mirrored", "opposite" }, false ) )
			loc = MakeMirrored( loc );
		auto name = loc.ConvertName( pn.get< string >( "source", "leg0" ) );

		if ( pn.get< string >( "type" ) != "Neuron" )
			name += '.' + pn.get< string >( "type" );

		auto iter = flut::find_if( m_InterNeurons, [&]( const InterNeuronUP& n ) { return n->name_ == name; } );
		return iter != m_InterNeurons.end() ? iter->get() : nullptr;
	}

	void NeuralController::StoreData( Storage<Real>::Frame& frame, const StoreDataFlags& flags )
	{
		for ( auto& n : m_SensorNeurons )
			frame[ "neuron." + n->name_ ] = n->output_;
		for ( auto& n : m_InterNeurons )
			frame[ "neuron." + n->name_ ] = n->output_;
	}

	String NeuralController::GetClassSignature() const
	{
		return flut::stringf( "N%d", m_SensorNeurons.size() + m_InterNeurons.size() );
	}
}
