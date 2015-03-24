#include "stdafx.h"

#include "StateController.h"
#include "../sim/Model.h"
#include "../sim/sim.h"
#include "../sim/Leg.h"
#include "../core/InitFromPropNode.h"

#include <boost/foreach.hpp>
#include <boost/assign.hpp>

#include "../sim/Body.h"
#include "../sim/Factories.h"
#include "../core/Log.h"

namespace scone
{
	namespace cs
	{
		std::map< StateController::LegState::State, String > g_StateNames = boost::assign::map_list_of
			( StateController::LegState::UnknownState, "Unknown" )
			( StateController::LegState::StanceState, "Stance" )
			( StateController::LegState::LiftoffState, "Liftoff" )
			( StateController::LegState::SwingState, "Swing" )
			( StateController::LegState::LandingState, "Landing" );

		const String& scone::cs::StateController::LegState::GetStateName( State state )
		{
			return g_StateNames[ state ];
		}

		StateController::StateController( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& target_area ) :
		sim::Controller( props, par, model, target_area )
		{
			INIT_FROM_PROP( props, contact_force_threshold, 10.0 );
			INIT_FROM_PROP( props, landing_offset, 0.1 );

			// create leg states
			BOOST_FOREACH( sim::LegUP& leg, model.GetLegs() )
				m_LegStates.push_back( LegStateUP( new LegState( *leg ) ) );

			// create additional controllers for each leg
			// TODO: allow definition of leg and state instances for each controller
			const PropNode& ccprops = props.GetChild( "ConditionalControllers" ).SetFlag();
			for ( PropNode::ConstChildIter it = ccprops.Begin(); it != ccprops.End(); ++it )
			{
				it->second->SetFlag();
				BOOST_FOREACH( sim::LegUP& leg, model.GetLegs() )
					m_ConditionalControllers.push_back( ConditionalControllerUP( new ConditionalController( *it->second, par, model, *leg ) ) );
			}
		}

		StateController::ConditionalController::ConditionalController( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Leg& leg ) :
		active( false ),
		active_since( 0.0 )
		{
			leg_index = leg.GetIndex();

			// load state_mask directly from PropNode (using streaming operator)
			INIT_FROM_PROP_REQUIRED( props, state_mask );

			// create controller
			const PropNode& cprops = props.GetChild( "Controller" ).SetFlag();
			par.PushNamePrefix( "S" + state_mask.to_string() + "." );

			// TODO: allow neater definition of target area instead of just taking the leg side
			controller = sim::CreateController( cprops, par, model, leg.GetSide() == LeftSide ? sim::Area::LEFT_SIDE : sim::Area::RIGHT_SIDE );
			par.PopNamePrefix();
		}

		StateController::~StateController()
		{
		}

		void StateController::UpdateControls( sim::Model& model, double timestamp )
		{
			if ( model.GetIntegrationStep() != model.GetPreviousIntegrationStep() )
			{
				// only update the states after a successful integration step
				UpdateLegStates( model, timestamp );
				UpdateControllerStates( model, timestamp );
			}

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
				ls.sagittal_pos = ls.leg.GetFootLink().GetBody().GetPos().x - ls.leg.GetBaseLink().GetBody().GetPos().x;
				ls.coronal_pos = ls.leg.GetFootLink().GetBody().GetPos().z - ls.leg.GetBaseLink().GetBody().GetPos().z;
			}

			// update states
			for ( size_t idx = 0; idx < m_LegStates.size(); ++idx )
			{
				LegState& ls = *m_LegStates[ idx ];
				LegState& mir_ls = *m_LegStates[ idx ^ 1 ];
				LegState::State new_state = ls.state;

				if ( ls.contact )
				{
					switch( ls.state )
					{
					case LegState::UnknownState:
					case LegState::StanceState:
					case LegState::SwingState:
					case LegState::LandingState:
						if ( mir_ls.contact && ls.sagittal_pos < mir_ls.sagittal_pos )
							new_state = LegState::LiftoffState;
						else new_state = LegState::StanceState;
						break;

					case LegState::LiftoffState:
						break;
					}
				}
				else
				{
					switch( ls.state )
					{
					case LegState::UnknownState:
					case LegState::LiftoffState:
						new_state = LegState::SwingState;
						break;

					case LegState::SwingState:
						// check Swing -> Landing
						if ( ls.sagittal_pos > landing_offset )
							new_state = LegState::LandingState;

					case LegState::StanceState:
					case LegState::LandingState:
						break;
					}
				}

				if ( new_state != ls.state )
				{
					log::Trace( "%.3f: Leg %d state changed from %s to %s", timestamp, idx, LegState::GetStateName( ls.state ).c_str(), LegState::GetStateName( new_state ).c_str() );
					ls.state = new_state;
				}
			}
		}

		void StateController::UpdateControllerStates( sim::Model& model, double timestamp )
		{
			// update controller states
			BOOST_FOREACH( ConditionalControllerUP& cc, m_ConditionalControllers )
			{
				bool activate = cc->state_mask.test( m_LegStates[ cc->leg_index ]->state );

				// activate or deactivate controller
				if ( activate != cc->active )
				{
					cc->active = activate; 
					cc->active_since = timestamp;
				}
			}
		}

		scone::String StateController::GetSignature()
		{
			return GetStringF( "SC%d.", m_ConditionalControllers.size() / 2 ) + m_ConditionalControllers.front()->controller->GetSignature();
		}
	}
}
