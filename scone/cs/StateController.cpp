#include "stdafx.h"

#include "StateController.h"
#include "../sim/Model.h"
#include "../sim/sim.h"
#include "../sim/Leg.h"
#include "../core/InitFromPropNode.h"

#include <boost/foreach.hpp>
#include "../sim/Body.h"

namespace scone
{
	namespace cs
	{
		StateController::LegState::LegState( const sim::Leg& l ) :
		leg( l ),
		state( UnknownState ),
		contact( false ),
		sagittal_pos( 0.0 ),
		coronal_pos( 0.0 )
		{
		}

		StateController::StateController( const PropNode& props ) :
		sim::Controller( props )
		{
			INIT_FROM_PROP( props, contact_force_threshold, 10.0 );
			INIT_FROM_PROP( props, contact_force_threshold, 10.0 );
		}

		StateController::~StateController()
		{
		}

		void StateController::Initialize( sim::Model& model, opt::ParamSet& par, const PropNode& props )
		{
			// create leg states
			BOOST_FOREACH( sim::LegUP& leg, model.GetLegs() )
				m_LegStates.push_back( LegStateUP( new LegState( *leg ) ) );
		}

		void StateController::UpdateControls( sim::Model& model, double timestamp )
		{
			UpdateLegStates( model, timestamp );
			UpdateControllerStates( model, timestamp );

			for ( auto it = m_Controllers.begin(); it != m_Controllers.end(); ++it )
			{
				if ( it->first.active )
					it->second->UpdateControls( model, timestamp - it->first.active_since );
			}
		}

		void StateController::UpdateLegStates( sim::Model& model, double timestamp )
		{
			// update statuses
			for ( size_t idx = 0; idx < m_LegStates.size(); ++idx )
			{
				LegState& ls = *m_LegStates[ idx ];
				ls.contact = ls.leg.GetContactForce().y > contact_force_threshold;
				ls.sagittal_pos = ls.leg.GetFootLink().GetBody().GetPos().x - model.GetRootLink().GetBody().GetPos().x;
				ls.coronal_pos = ls.leg.GetFootLink().GetBody().GetPos().z - model.GetRootLink().GetBody().GetPos().z;
			}

			// update states
			for ( size_t idx = 0; idx < m_LegStates.size(); ++idx )
			{
				LegState& ls = *m_LegStates[ idx ];
				LegState& mir_ls = *m_LegStates[ idx ^ 1 ];

				switch( ls.state )
				{
				case LegState::StanceState:
					// check for liftoff
					if ( mir_ls.contact && ls.sagittal_pos < mir_ls.sagittal_pos )
						ls.state = LegState::LiftoffState;
					break;
				case LegState::LiftoffState:
					if ( !ls.contact )
						ls.state = LegState::SwingState;
					break;
				case LegState::SwingState:
					if ( ls.contact )
						ls.state = LegState::StanceState;
					break;
				case LegState::LandingState:
					if ( ls.contact )
						ls.state = LegState::StanceState;
					break;
				}
			}
		}

		void StateController::UpdateControllerStates( sim::Model& model, double timestamp )
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
