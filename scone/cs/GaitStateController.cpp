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
		EnumStringMap< GaitStateController::LegInfo::GaitState > GaitStateController::LegInfo::m_StateNames = EnumStringMap< GaitStateController::LegInfo::GaitState >(
			GaitStateController::LegInfo::UnknownState, "Unknown",
			GaitStateController::LegInfo::EarlyStanceState, "EarlyStance",
			GaitStateController::LegInfo::LateStanceState, "LateStance",
			GaitStateController::LegInfo::LiftoffState, "Liftoff",
			GaitStateController::LegInfo::SwingState, "Swing",
			GaitStateController::LegInfo::LandingState, "Landing"
			);

		GaitStateController::GaitStateController( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& target_area ) :
		sim::Controller( props, par, model, target_area )
		{
			INIT_FROM_PROP( props, contact_force_threshold, 10.0 );
			INIT_FROM_PROP_PAR( props, par, landing_threshold );
			INIT_FROM_PROP_PAR( props, par, late_stance_threshold );
			
			// create leg states
			BOOST_FOREACH( sim::LegUP& leg, model.GetLegs() )
			{		
				m_LegStates.push_back( LegStateUP( new LegInfo( *leg ) ) );
				log::TraceF( "leg %d leg_length=%.2f", m_LegStates.back()->leg.GetIndex(), m_LegStates.back()->leg_length );
			}

			// create instances for each controller
			const PropNode& ccProps = props.GetChild( "ConditionalControllers" ).Touch();
			for ( PropNode::ConstChildIter ccIt = ccProps.Begin(); ccIt != ccProps.End(); ++ccIt )
			{
				ccIt->second->Touch();
				const PropNode& instProps = ccIt->second->GetChild( "Instances" ).Touch();

				for ( PropNode::ConstChildIter instIt = instProps.Begin(); instIt != instProps.End(); ++instIt )
				{
					// automatically create controllers for all legs
					for ( size_t legIdx = 0; legIdx < model.GetLegs().size(); ++legIdx )
					{
						// create new conditional controller
						m_ConditionalControllers.push_back( ConditionalControllerUP( new ConditionalController() ) );
						ConditionalController& cc = *m_ConditionalControllers.back();

						// initialize state_mask based on names
						String states = instIt->second->GetStr( "state_mask" );
						for ( int i = 0; i < LegInfo::StateCount; ++i )
							cc.state_mask.set( i, states.find( LegInfo::m_StateNames.GetString( LegInfo::GaitState( i ) ) ) != String::npos );
						SCONE_CONDITIONAL_THROW( !cc.state_mask.any(), "Conditional Controller has empty state mask" )

						// initialize leg index
						cc.leg_index = legIdx;

						// create controller
						// TODO: allow neater definition of target area instead of just taking the leg side
						const PropNode& cprops = ccIt->second->GetChild( "Controller" );
						par.PushNamePrefix( "S" + cc.state_mask.to_string() + "." );
						cc.controller = sim::CreateController( cprops, par, model, model.GetLeg( cc.leg_index ).GetSide() == LeftSide ? sim::Area::LEFT_SIDE : sim::Area::RIGHT_SIDE );
						par.PopNamePrefix();
					}
				}
			}
		}

		GaitStateController::~GaitStateController()
		{
		}

		void GaitStateController::UpdateControls( sim::Model& model, double timestamp )
		{
			//log::TraceF( "%d %.6f %.6f", model.GetIntegrationStep(), timestamp, model.GetDeltaTime() );

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
				LegInfo& ls = *m_LegStates[ idx ];
				ls.contact = ls.leg.GetContactForce().y > contact_force_threshold;
				ls.sagittal_pos = ls.leg.GetFootLink().GetBody().GetPos().x - ls.leg.GetBaseLink().GetBody().GetPos().x;
				ls.coronal_pos = ls.leg.GetFootLink().GetBody().GetPos().z - ls.leg.GetBaseLink().GetBody().GetPos().z;
			}

			// update states
			for ( size_t idx = 0; idx < m_LegStates.size(); ++idx )
			{
				LegInfo& ls = *m_LegStates[ idx ];
				LegInfo& mir_ls = *m_LegStates[ idx ^ 1 ];
				LegInfo::GaitState new_state = ls.state;

				switch( ls.state )
				{
				case LegInfo::UnknownState:
					if ( ls.contact )
					{
						if ( mir_ls.contact && ls.sagittal_pos < mir_ls.sagittal_pos )
							new_state = LegInfo::LiftoffState;
						else if ( ls.sagittal_pos < ls.leg_length * late_stance_threshold )
							new_state = LegInfo::LateStanceState;
						else new_state = LegInfo::EarlyStanceState;
					}
					else
					{
						if ( ls.sagittal_pos > ls.leg_length * landing_threshold )
							new_state = LegInfo::LandingState;
						else new_state = LegInfo::SwingState;
					}
					break;

				case LegInfo::EarlyStanceState:
					if ( mir_ls.contact && ls.sagittal_pos < mir_ls.sagittal_pos )
						new_state = LegInfo::LiftoffState;
					else if ( ls.sagittal_pos < ls.leg_length * late_stance_threshold )
						new_state = LegInfo::LateStanceState;
					break;

				case LegInfo::LateStanceState:
					if ( mir_ls.contact && ls.sagittal_pos < mir_ls.sagittal_pos )
						new_state = LegInfo::LiftoffState;
					break;

				case LegInfo::LiftoffState:
					if ( !ls.contact )
						new_state = LegInfo::SwingState;
					break;

				case LegInfo::SwingState:
					if ( ls.contact && ls.sagittal_pos > mir_ls.sagittal_pos )
						new_state = LegInfo::EarlyStanceState;
					if ( !ls.contact && ls.sagittal_pos > ls.leg_length * landing_threshold )
						new_state = LegInfo::LandingState;
					break;

				case LegInfo::LandingState:
					if ( ls.contact )
						new_state = LegInfo::EarlyStanceState;
					break;
				}

				if ( new_state != ls.state )
				{
					log::TraceF( "%.3f: Leg %d state changed from %s to %s", timestamp, idx, ls.GetStateName().c_str(), LegInfo::m_StateNames.GetString( new_state ).c_str() );
					ls.state = new_state;
				}
			}
		}

		void GaitStateController::UpdateControllerStates( sim::Model& model, double timestamp )
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

		scone::String GaitStateController::GetSignature()
		{
			String s = "gs";

			std::map< String, int > controllers;
			BOOST_FOREACH( ConditionalControllerUP& cc, m_ConditionalControllers )
				controllers[ cc->controller->GetSignature() ] += 1;

			// output number of controllers per leg
			for ( auto it = controllers.begin(); it != controllers.end(); ++it )
				s += "_" + ToString( it->second / m_LegStates.size() ) + it->first;

			return s;
		}
	}
}
