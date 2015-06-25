#include "stdafx.h"

#include "GaitStateController.h"
#include "../sim/Model.h"
#include "../sim/sim.h"
#include "../sim/Leg.h"
#include "../sim/Muscle.h"
#include "../core/InitFromPropNode.h"

#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>

#include "../sim/Body.h"
#include "../sim/Factories.h"
#include "../core/Log.h"
#include "../core/Profiler.h"
#include "../sim/SensorDelayAdapter.h"

namespace scone
{
	namespace cs
	{
		StringMap< GaitStateController::LegState::GaitState > GaitStateController::LegState::m_StateNames = StringMap< GaitStateController::LegState::GaitState >(
			GaitStateController::LegState::UnknownState, "Unknown",
			GaitStateController::LegState::EarlyStanceState, "EarlyStance",
			GaitStateController::LegState::LateStanceState, "LateStance",
			GaitStateController::LegState::LiftoffState, "Liftoff",
			GaitStateController::LegState::SwingState, "Swing",
			GaitStateController::LegState::LandingState, "Landing"
			);


		GaitStateController::LegState::LegState( sim::Leg& l ) :
		leg( l ),
		state( UnknownState ),
		contact( false ),
		sagittal_pos( 0.0 ),
		coronal_pos( 0.0 ),
		leg_length( l.MeasureLength() ),
		load_sensor( l.GetModel().AcquireDelayedSensor< sim::LegLoadSensor >( l ) )
		{ }

		GaitStateController::GaitStateController( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& target_area ) :
		Controller( props, par, model, target_area )
		{
			// TODO: move contact_force_threshold to leg?
			INIT_PARAM( props, par, stance_load_threshold, 0.1 );
			INIT_PARAM( props, par, landing_threshold, 0.0 );
			INIT_PARAM( props, par, late_stance_threshold, 0.0 );
			INIT_PROPERTY( props, leg_load_sensor_delay, 0.0 );
			
			// create leg states
			BOOST_FOREACH( sim::LegUP& leg, model.GetLegs() )
			{		
				m_LegStates.push_back( LegStateUP( new LegState( *leg ) ) );
				log::TraceF( "leg %d leg_length=%.2f", m_LegStates.back()->leg.GetIndex(), m_LegStates.back()->leg_length );
			}

			// create instances for each controller
			const PropNode& ccProps = props.GetChild( "ConditionalControllers" );
			for ( PropNode::ConstChildIter ccIt = ccProps.Begin(); ccIt != ccProps.End(); ++ccIt )
			{
				// get state masks
				String state_masks = ccIt->second->GetStr( "states" );
				boost::char_separator< char > state_mask_seperator(";,");
				boost::tokenizer< boost::char_separator< char > > state_tokens( state_masks, state_mask_seperator );
				BOOST_FOREACH( const String& instance_states, state_tokens )
				{
					// automatically create controllers for all legs (sides)
					for ( size_t legIdx = 0; legIdx < model.GetLegs().size(); ++legIdx )
					{
						// create new conditional controller
						m_ConditionalControllers.push_back( ConditionalControllerUP( new ConditionalController() ) );
						ConditionalController& cc = *m_ConditionalControllers.back();

						// initialize state_mask based on names in instance_states (TODO: use tokenizer?)
						for ( int i = 0; i < LegState::StateCount; ++i )
							cc.state_mask.set( i, instance_states.find( LegState::m_StateNames.GetString( LegState::GaitState( i ) ) ) != String::npos );
						SCONE_THROW_IF( !cc.state_mask.any(), "Conditional Controller has empty state mask" )

						// initialize leg index
						cc.leg_index = legIdx;

						// create controller
						// TODO: allow neater definition of target area instead of just taking the leg side
						const PropNode& cprops = ccIt->second->GetChild( "Controller" );
						opt::ScopedParamSetPrefixer prefixer( par, "S" + cc.state_mask.to_string() + "." );
						cc.controller = sim::CreateController( cprops, par, model, model.GetLeg( cc.leg_index ).GetSide() == LeftSide ? sim::Area::LEFT_SIDE : sim::Area::RIGHT_SIDE );
					}
				}
			}
		}

		GaitStateController::~GaitStateController()
		{
		}

		sim::Controller::UpdateResult GaitStateController::UpdateControls( sim::Model& model, double timestamp )
		{
			SCONE_PROFILE_SCOPE;

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

			return SuccessfulUpdate;
		}

		void GaitStateController::UpdateLegStates( sim::Model& model, double timestamp )
		{
			SCONE_PROFILE_SCOPE;

			// update statuses
			for ( size_t idx = 0; idx < m_LegStates.size(); ++idx )
			{
				LegState& ls = *m_LegStates[ idx ];
				//ls.contact = ls.load_sensor.GetValue( leg_load_sensor_delay ) >= stance_load_threshold;
				ls.contact = ls.leg.GetLoad() >= stance_load_threshold;
				ls.sagittal_pos = ls.leg.GetFootLink().GetBody().GetPos().x - ls.leg.GetBaseLink().GetBody().GetPos().x;
				ls.coronal_pos = ls.leg.GetFootLink().GetBody().GetPos().z - ls.leg.GetBaseLink().GetBody().GetPos().z;
			}

			// update states
			for ( size_t idx = 0; idx < m_LegStates.size(); ++idx )
			{
				LegState& ls = *m_LegStates[ idx ];
				LegState& mir_ls = *m_LegStates[ idx ^ 1 ];
				LegState::GaitState new_state = ls.state;

				switch( ls.state )
				{
				case LegState::UnknownState:
					if ( ls.contact )
					{
						if ( mir_ls.contact && ls.sagittal_pos < mir_ls.sagittal_pos )
							new_state = LegState::LiftoffState;
						else if ( ls.sagittal_pos < ls.leg_length * late_stance_threshold )
							new_state = LegState::LateStanceState;
						else new_state = LegState::EarlyStanceState;
					}
					else
					{
						if ( ls.sagittal_pos > ls.leg_length * landing_threshold )
							new_state = LegState::LandingState;
						else new_state = LegState::SwingState;
					}
					break;

				case LegState::EarlyStanceState:
					if ( mir_ls.contact && ls.sagittal_pos < mir_ls.sagittal_pos )
						new_state = LegState::LiftoffState;
					else if ( ls.sagittal_pos < ls.leg_length * late_stance_threshold )
						new_state = LegState::LateStanceState;
					break;

				case LegState::LateStanceState:
					if ( mir_ls.contact && ls.sagittal_pos < mir_ls.sagittal_pos )
						new_state = LegState::LiftoffState;
					break;

				case LegState::LiftoffState:
					if ( !ls.contact )
						new_state = LegState::SwingState;
					break;

				case LegState::SwingState:
					if ( ls.contact && ls.sagittal_pos > mir_ls.sagittal_pos )
						new_state = LegState::EarlyStanceState;
					if ( !ls.contact && ls.sagittal_pos > ls.leg_length * landing_threshold )
						new_state = LegState::LandingState;
					break;

				case LegState::LandingState:
					if ( ls.contact )
						new_state = LegState::EarlyStanceState;
					break;
				}

				if ( new_state != ls.state )
				{
					log::TraceF( "%.3f: Leg %d state changed from %s to %s", timestamp, idx, ls.GetStateName().c_str(), LegState::m_StateNames.GetString( new_state ).c_str() );
					ls.state = new_state;
				}
			}
		}

		void GaitStateController::UpdateControllerStates( sim::Model& model, double timestamp )
		{
			SCONE_PROFILE_SCOPE;

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

		scone::String GaitStateController::GetClassSignature() const
		{
			String s = "G";

			std::map< String, int > controllers;
			BOOST_FOREACH( const ConditionalControllerUP& cc, m_ConditionalControllers )
				controllers[ cc->controller->GetSignature() ] += 1;

			// output number of controllers per leg
			for ( auto it = controllers.begin(); it != controllers.end(); ++it )
				s += "_" + ToString( it->second / m_LegStates.size() ) + it->first;

			return s;
		}
	}
}
