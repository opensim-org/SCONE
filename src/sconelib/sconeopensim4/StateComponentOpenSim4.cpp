/*
** StateComponentOpenSim4.cpp
**
** Copyright (C) 2013-2021 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#ifdef ENABLE_STATE_COMPONENTS

#include "StateComponentOpenSim4.h"
#include "SimTKcommon/internal/EventHandler.h"
#include "scone/model/StateComponent.h"

using namespace scone;

namespace OpenSim
{
	StateComponentOpenSim4::EventHandler::EventHandler(
		StateComponentOpenSim4* modelComponent, StateComponent* stateComponent )
		: SimTK::TriggeredEventHandler( SimTK::Stage::Dynamics ),
		m_modelComponent( modelComponent ), m_stateComponent( stateComponent )
	{
		if ( m_stateComponent->TriggeredOnSign() > 0 )
		{
			getTriggerInfo().setTriggerOnRisingSignTransition( true );
			getTriggerInfo().setTriggerOnFallingSignTransition( false );
		}
		else if ( m_stateComponent->TriggeredOnSign() < 0 )
		{
			getTriggerInfo().setTriggerOnRisingSignTransition( false );
			getTriggerInfo().setTriggerOnFallingSignTransition( true );
		}
		else
		{
			getTriggerInfo().setTriggerOnRisingSignTransition( true );
			getTriggerInfo().setTriggerOnFallingSignTransition( true );
		}
	}

	SimTK::Real StateComponentOpenSim4::EventHandler::getValue( const SimTK::State& s ) const
	{
		auto x = m_modelComponent->getStateVariables( s );
		return m_stateComponent->CheckForEvent( s.getTime(), x );
	}
	void StateComponentOpenSim4::EventHandler::handleEvent( SimTK::State& s,
		SimTK::Real accuracy,
		bool& shouldTerminate ) const
	{
		auto x = m_modelComponent->getStateVariables( s );
		auto xNew = m_stateComponent->EventHandler( s.getTime(), x );
		m_modelComponent->setStateVariables( s, xNew );
	}

	StateComponentOpenSim4::StateComponentOpenSim4( StateComponent* stateComponent ) :
		ModelComponent(),
		m_stateComponent( stateComponent )
	{
		for ( int i = 0; i < m_stateComponent->GetInitialConditions().size(); i++ )
			m_stateVariables.push_back( m_stateComponent->GetName() + "_" + std::to_string( i ) );
	}

	std::vector<Real> StateComponentOpenSim4::getStateVariables( const SimTK::State& s ) const
	{
		std::vector< Real > state;
		for ( auto stateVariable : m_stateVariables )
			state.push_back( getStateVariable( s, stateVariable ) );

		return state;
	}

	void StateComponentOpenSim4::setStateVariables( SimTK::State& s, std::vector< Real > x0 ) const
	{
		for ( int i = 0; i < x0.size(); i++ )
			setStateVariable( s, m_stateVariables[ i ], x0[ i ] );
	}

	SimTK::Vector StateComponentOpenSim4::computeStateVariableDerivatives( const SimTK::State& s ) const
	{
		auto x0 = getStateVariables( s );
		auto dxdt = m_stateComponent->CalcStateDerivatives( s.getTime(), x0 );
		SimTK::Vector result( int( dxdt.size() ), 0.0 );
		for ( int i = 0; i < dxdt.size(); i++ )
			result[ i ] = dxdt[ i ];

		return result;
	}

	void StateComponentOpenSim4::initStateFromProperties( SimTK::State& s ) const
	{
		Super::initStateFromProperties( s );
		setStateVariables( s, m_stateComponent->GetInitialConditions() );
	}

	void StateComponentOpenSim4::addToSystem( SimTK::MultibodySystem& system ) const
	{
		Super::addToSystem( system );

		// define state variables
		for ( auto stateVariable : m_stateVariables )
			addStateVariable( stateVariable, SimTK::Stage::Dynamics );

		// add an event function if the system is hybrid
		if ( m_stateComponent->HasDiscreteEvent() )
		{
			auto self = const_cast<StateComponentOpenSim4*>( this );
			auto event = new EventHandler( self, self->m_stateComponent );
			system.addEventHandler( event );
		}
	}
}
#endif // ENABLE_STATE_COMPONENTS
