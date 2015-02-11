#include "stdafx.h"

#include "StateController.h"
#include "../sim/Model.h"
#include "../sim/sim.h"
#include "../sim/Leg.h"
#include "../core/InitFromPropNode.h"

#include <boost/foreach.hpp>

namespace scone
{
	namespace cs
	{
		StateController::StateController( const PropNode& props ) :
		sim::Controller( props )
		{
			INIT_FROM_PROP( props, contact_force_threshold, 10.0 );
		}

		StateController::~StateController()
		{
		}

		void StateController::Initialize( sim::Model& model )
		{
			// create leg states
			BOOST_FOREACH( sim::LegUP& leg, model.GetLegs() )
				m_LegStates.push_back( LegStateUP( new LegState( *leg ) ) );
		}

		void StateController::ProcessParameters( opt::ParamSet& par )
		{
		}

		void StateController::UpdateControls( sim::Model& model, double timestamp )
		{
			UpdateLegStates( timestamp );
			UpdateControllerStates( timestamp );

			for ( auto it = m_Controllers.begin(); it != m_Controllers.end(); ++it )
			{
				if ( it->first.active )
					it->second->UpdateControls( model, timestamp - it->first.active_since );
			}
		}

		void StateController::UpdateLegStates( double timestamp )
		{
			for ( size_t idx = 0; idx < m_LegStates.size(); ++idx )
			{
				LegState& ls = *m_LegStates[ idx ];

				// check contact
				if ( ls.leg.GetContactForce().y > contact_force_threshold )
					ls.state.Set( LegState::StanceState, timestamp );
				else ls.state.Set( LegState::SwingState, timestamp );
			}
		}

		void StateController::UpdateControllerStates( double timestamp )
		{
			// update controller states
			for ( size_t cidx = 0; cidx < m_Controllers.size(); ++cidx )
			{
				ControllerState& cstate = m_Controllers[ cidx ].first;
				bool activate = false;
				for ( size_t lidx = 0; lidx < m_LegStates.size(); ++lidx )
				{
					if ( cstate.leg_condition[ lidx ].test( m_LegStates[ lidx ]->state ) )
					{
						activate = true;
						break;
					}
				}

				if ( activate && !cstate.active )
				{
					cstate.active = true; // activate controller
					cstate.active_since = timestamp;
				}
				else if ( cstate.active )
					cstate.active = false; // deactivate controller
			}
		}
	}
}
