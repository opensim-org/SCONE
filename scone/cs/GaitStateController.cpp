#include "stdafx.h"

#include "GaitStateController.h"
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
		EnumStringMap< GaitStateController::LegState::Phase > GaitStateController::LegState::m_PhaseNames = EnumStringMap< GaitStateController::LegState::Phase >(
			GaitStateController::LegState::UnknownState, "Unknown",
			GaitStateController::LegState::StanceState, "Stance",
			GaitStateController::LegState::LiftoffState, "Liftoff",
			GaitStateController::LegState::SwingState, "Swing",
			GaitStateController::LegState::LandingState, "Landing"
			);

		GaitStateController::GaitStateController( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& target_area ) :
		sim::Controller( props, par, model, target_area )
		{
			INIT_FROM_PROP( props, contact_force_threshold, 10.0 );
			landing_threshold = par.Get( "landing_threshold", props.GetChild( "landing_threshold" ).SetFlag() );
			
			// create leg states
			BOOST_FOREACH( sim::LegUP& leg, model.GetLegs() )
				m_LegStates.push_back( LegStateUP( new LegState( *leg ) ) );

			// create instances for each controller
			const PropNode& ccProps = props.GetChild( "ConditionalControllers" ).SetFlag();
			for ( PropNode::ConstChildIter ccIt = ccProps.Begin(); ccIt != ccProps.End(); ++ccIt )
			{
				ccIt->second->SetFlag();
				const PropNode& instProps = ccIt->second->GetChild( "Instances" ).SetFlag();

				for ( PropNode::ConstChildIter instIt = instProps.Begin(); instIt != instProps.End(); ++instIt )
					m_ConditionalControllers.push_back( ConditionalControllerUP( new ConditionalController( *ccIt->second, par, model, instIt->second->SetFlag() ) ) );
			}
		}

		GaitStateController::ConditionalController::ConditionalController( const PropNode& props, opt::ParamSet& par, sim::Model& model, const PropNode& instance ) :
		active( false ),
		active_since( 0.0 )
		{
			// load leg_index and state_mask directly from PropNode (using streaming operator)
			INIT_FROM_PROP_REQUIRED( instance, leg_index );
			INIT_FROM_PROP_REQUIRED( instance, phase_mask );

			// create controller
			const PropNode& cprops = props.GetChild( "Controller" ).SetFlag();
			par.PushNamePrefix( "S" + phase_mask.to_string() + "." );

			// TODO: allow neater definition of target area instead of just taking the leg side
			controller = sim::CreateController( cprops, par, model, model.GetLeg( leg_index ).GetSide() == LeftSide ? sim::Area::LEFT_SIDE : sim::Area::RIGHT_SIDE );
			par.PopNamePrefix();
		}

		GaitStateController::~GaitStateController()
		{
		}

		void GaitStateController::UpdateControls( sim::Model& model, double timestamp )
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

		void GaitStateController::UpdateLegStates( sim::Model& model, double timestamp )
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
				LegState::Phase new_state = ls.phase;

				switch( ls.phase )
				{
				case LegState::UnknownState:
					if ( ls.contact )
					{
						if ( mir_ls.contact && ls.sagittal_pos < mir_ls.sagittal_pos )
							new_state = LegState::LiftoffState;
						else new_state = LegState::StanceState;
					}
					else
					{
						if ( ls.sagittal_pos > landing_threshold )
							new_state = LegState::LandingState;
						else new_state = LegState::SwingState;
					}
					break;

				case LegState::StanceState:
					if ( mir_ls.contact && ls.sagittal_pos < mir_ls.sagittal_pos )
						new_state = LegState::LiftoffState;
					break;

				case LegState::LiftoffState:
					if ( !ls.contact )
						new_state = LegState::SwingState;
					break;

				case LegState::SwingState:
					if ( ls.contact && ls.sagittal_pos > mir_ls.sagittal_pos )
						new_state = LegState::StanceState;
					if ( !ls.contact && ls.sagittal_pos > landing_threshold )
						new_state = LegState::LandingState;
					break;

				case LegState::LandingState:
					if ( ls.contact )
						new_state = LegState::StanceState;
					break;
				}

				if ( new_state != ls.phase )
				{
					log::Trace( "%.3f: Leg %d state changed from %s to %s", timestamp, idx, ls.GetPhaseName().c_str(), LegState::m_PhaseNames.GetString( new_state ).c_str() );
					ls.phase = new_state;
				}
			}
		}

		void GaitStateController::UpdateControllerStates( sim::Model& model, double timestamp )
		{
			// update controller states
			BOOST_FOREACH( ConditionalControllerUP& cc, m_ConditionalControllers )
			{
				bool activate = cc->phase_mask.test( m_LegStates[ cc->leg_index ]->phase );

				// activate or deactivate controller
				if ( activate != cc->active )
				{
					cc->active = activate; 
					cc->active_since = timestamp;
				}
			}
		}

		scone::String GaitStateController::GetSignature()
		{
			return GetStringF( "SC%d.", m_ConditionalControllers.size() / 2 ) + m_ConditionalControllers.front()->controller->GetSignature();
		}
	}
}
