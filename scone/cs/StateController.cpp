#include "stdafx.h"

#include "StateController.h"
#include "../sim/Model.h"
#include "../sim/sim.h"
#include "../sim/Leg.h"
#include "../core/InitFromPropNode.h"

#include <boost/foreach.hpp>
#include "../sim/Body.h"
#include "../sim/Factories.h"

namespace scone
{
	namespace cs
	{
		StateController::StateController( const PropNode& props, opt::ParamSet& par, sim::Model& model ) :
		sim::Controller( props, par, model )
		{
			INIT_FROM_PROP( props, contact_force_threshold, 10.0 );

			// create leg states
			BOOST_FOREACH( sim::LegUP& leg, model.GetLegs() )
				m_LegStates.push_back( LegStateUP( new LegState( *leg ) ) );

			// create additional controllers
			const PropNode& ccprops = props.GetChild( "ConditionalControllers" );
			for ( PropNode::ConstChildIter it = ccprops.Begin(); it != ccprops.End(); ++it )
			{
				m_ConditionalControllers.push_back( ConditionalControllerUP( new ConditionalController( props, par, model ) ) );
			}
		}

		StateController::ConditionalController::ConditionalController( const PropNode& props, opt::ParamSet& par, sim::Model& model ) :
		active( false ),
		active_since( 0.0 )
		{
			const PropNode& cprops = props.GetChild( "controller" );
			controller = sim::CreateController( cprops, par, model );
		}


		StateController::~StateController()
		{
		}

		void StateController::UpdateControls( sim::Model& model, double timestamp )
		{
			UpdateLegStates( model, timestamp );
			UpdateControllerStates( model, timestamp );

			BOOST_FOREACH( ConditionalControllerUP& cc, m_ConditionalControllers )
			{
				if ( cc->active )
					cc->controller->UpdateControls( model, timestamp - cc->active_since );
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
			BOOST_FOREACH( ConditionalControllerUP& cc, m_ConditionalControllers )
			{
				bool activate = false;
				for ( size_t lidx = 0; lidx < m_LegStates.size(); ++lidx )
				{
					if ( cc->leg_condition[ lidx ].test( m_LegStates[ lidx ]->state ) )
					{
						activate = true;
						break;
					}
				}

				// activate or deactivate controller
				if ( activate != cc->active )
				{
					cc->active = activate; 
					cc->active_since = timestamp;
				}
			}
		}
	}
}
