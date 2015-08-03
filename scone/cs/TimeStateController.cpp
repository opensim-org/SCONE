#include "stdafx.h"
#include "TimeStateController.h"
#include "../core/InitFromPropNode.h"

namespace scone
{
	namespace cs
	{
		TimeStateController::TimeStateController( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area ) :
		StateController( props, par, model, area )
		{
			// create states
			const PropNode& states_pn = props.GetChild( "TimeStates" );
			for ( auto it = states_pn.Begin(); it != states_pn.End(); ++it )
				m_States.push_back( TimeState( *it->second, par ) );

			std::sort( m_States.begin(), m_States.end() );
			SCONE_ASSERT( m_States.size() >= 2 );

			// create conditional controllers
			CreateConditionalControllers( props, par, model, area );

			// init initial state
			UpdateCurrentState( 0.0 );
		}

		TimeStateController::~TimeStateController()
		{
		}

		TimeStateController::TimeState::TimeState( const PropNode& pn, opt::ParamSet& par )
		{
			INIT_PROPERTY_REQUIRED( pn, name );
			opt::ScopedParamSetPrefixer prefixer( par, name + "." );

			INIT_PARAM_REQUIRED( pn, par, start_time );
		}

		TimeStateController::UpdateResult TimeStateController::UpdateAnalysis( const sim::Model& model, double timestamp )
		{
			UpdateCurrentState( timestamp );

			return SuccessfulUpdate;
		}

		void TimeStateController::UpdateCurrentState( double timestamp )
		{
			// states are sorted by time, so the current state can be found by searching back to front
			for ( m_CurrentState = GetStateCount() - 1; m_CurrentState > 0; --m_CurrentState )
				if ( m_States[ m_CurrentState ].start_time <= timestamp )
					break;

			UpdateConditionalControllerStates( m_CurrentState, timestamp );
		}
	}
}
