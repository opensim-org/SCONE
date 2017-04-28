#include "TimeStateController.h"
#include "scone/core/propnode_tools.h"

namespace scone
{
	TimeStateController::TimeStateController( const PropNode& props, ParamSet& par, Model& model, const Locality& area ) :
		StateController( props, par, model, area )
	{
		// create states
		const PropNode& states_pn = props.get_child( "TimeStates" );
		for ( auto it = states_pn.begin(); it != states_pn.end(); ++it )
			m_States.push_back( TimeState( it->second, par ) );

		std::sort( m_States.begin(), m_States.end() );
		SCONE_ASSERT( m_States.size() >= 1 );

		// create conditional controllers
		CreateConditionalControllers( props, par, model, area );

		// init initial state
		UpdateCurrentState( model, 0.0 );
	}

	TimeStateController::TimeState::TimeState( const PropNode& pn, ParamSet& par )
	{
		INIT_PROPERTY_REQUIRED( pn, name );
		ScopedParamSetPrefixer prefixer( par, name + "." );

		INIT_PARAM_REQUIRED( pn, par, start_time );
	}

	scone::String TimeStateController::GetClassSignature() const
	{
		return "T" + StateController::GetClassSignature();
	}

	scone::StateController::StateIndex TimeStateController::GetCurrentState( Model& model, double timestamp )
	{
		// states are sorted by time, so the current state can be found by searching back to front
		SCONE_ASSERT( GetStateCount() > 0 );
		StateIndex s = GetStateCount() - 1;
		while ( m_States[ s ].start_time > timestamp && s > 0 )
			--s;
		return s;
	}
}
