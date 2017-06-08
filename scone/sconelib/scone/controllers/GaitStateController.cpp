#include "GaitStateController.h"
#include "scone/model/Model.h"
#include "scone/model/Leg.h"
#include "scone/model/Muscle.h"
#include "scone/core/propnode_tools.h"

#include "scone/model/Body.h"
#include "scone/core/Log.h"
#include "scone/core/Profiler.h"
#include "scone/model/SensorDelayAdapter.h"
#include "scone/core/Factories.h"

namespace scone
{
	StringMap< GaitStateController::LegState::GaitState > GaitStateController::LegState::m_StateNames = StringMap< GaitStateController::LegState::GaitState >(
		GaitStateController::LegState::UnknownState, "Unknown",
		GaitStateController::LegState::EarlyStanceState, "EarlyStance",
		GaitStateController::LegState::LateStanceState, "LateStance",
		GaitStateController::LegState::LiftoffState, "Liftoff",
		GaitStateController::LegState::SwingState, "Swing",
		GaitStateController::LegState::LandingState, "Landing"
		);

	GaitStateController::LegState::LegState( Leg& l ) :
		leg( l ),
		state( UnknownState ),
		allow_stance_transition( false ),
		allow_swing_transition( false ),
		sagittal_pos( 0.0 ),
		coronal_pos( 0.0 ),
		leg_length( l.GetLength() ),
		load_sensor( l.GetModel().AcquireDelayedSensor< LegLoadSensor >( l ) )
	{ }

	GaitStateController::GaitStateController( const PropNode& props, Params& par, Model& model, const Locality& target_area ) :
		Controller( props, par, model, target_area )
	{
		// TODO: move contact_force_threshold to leg?
		INIT_PARAM( props, par, stance_load_threshold, 0.1 );
		INIT_PARAM( props, par, swing_load_threshold, stance_load_threshold );
		INIT_PARAM( props, par, landing_threshold, 0.0 );
		INIT_PARAM( props, par, late_stance_threshold, 0.0 );
		INIT_PARAM( props, par, liftoff_threshold, -1.0 ); // default value is such that parameter has no effect
		INIT_PROPERTY( props, leg_load_sensor_delay, 0.0 );
		INIT_PROPERTY( props, override_leg_length, 0.0 );

		// create leg states
		for ( LegUP& leg : model.GetLegs() )
		{
			m_LegStates.push_back( LegStateUP( new LegState( *leg ) ) );
			if ( override_leg_length != 0.0 )
				m_LegStates.back()->leg_length = override_leg_length;
			log::TraceF( "leg %d leg_length=%.5f", m_LegStates.back()->leg.GetIndex(), m_LegStates.back()->leg_length );
		}

		// create instances for each controller
		const PropNode& ccProps = props.get_child( "ConditionalControllers" );
		for ( PropNode::const_iterator ccIt = ccProps.begin(); ccIt != ccProps.end(); ++ccIt )
		{
			// get state masks
			String state_masks = ccIt->second.get< String >( "states" );
			auto state_tokens = flut::split_str( state_masks, ";," );
			for ( const String& instance_states : state_tokens )
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

					// TODO: allow neater definition of target area instead of just taking the leg side
					Locality a = Locality( model.GetLeg( cc.leg_index ).GetSide() );

					// create controller
					log::trace( "Creating controllers for " + GetConditionName( cc ) );
					const PropNode& cprops = ccIt->second.get_child( "Controller" );
					ScopedParamSetPrefixer prefixer( par, "S" + cc.state_mask.to_string() + "." );
					cc.controller = CreateController( cprops, par, model, a );
				}
			}
		}
		log::trace( "Controller created" );
	}

	GaitStateController::~GaitStateController()
	{
	}

	Controller::UpdateResult GaitStateController::UpdateControls( Model& model, double timestamp )
	{
		SCONE_PROFILE_FUNCTION;

		if ( model.GetIntegrationStep() != model.GetPreviousIntegrationStep() )
		{
			// only update the states after a successful integration step
			UpdateLegStates( model, timestamp );
			UpdateControllerStates( model, timestamp );
		}

		for ( ConditionalControllerUP& cc : m_ConditionalControllers )
		{
			if ( cc->active )
			{
				//log::Trace( "Updating Controls of " + GetConditionName( *cc ) );
				cc->controller->UpdateControls( model, timestamp - cc->active_since );
			}
		}

		return SuccessfulUpdate;
	}

	void GaitStateController::UpdateLegStates( Model& model, double timestamp )
	{
		SCONE_PROFILE_FUNCTION;

		// update statuses
		for ( size_t idx = 0; idx < m_LegStates.size(); ++idx )
		{
			LegState& ls = *m_LegStates[ idx ];
			ls.leg_load = ls.load_sensor.GetValue( leg_load_sensor_delay );
			ls.allow_stance_transition = ls.load_sensor.GetValue( leg_load_sensor_delay ) > stance_load_threshold;
			ls.allow_swing_transition = ls.load_sensor.GetValue( leg_load_sensor_delay ) <= swing_load_threshold;
			ls.sagittal_pos = ls.leg.GetFootLink().GetBody().GetComPos().x - ls.leg.GetBaseLink().GetBody().GetComPos().x;
			ls.coronal_pos = ls.leg.GetFootLink().GetBody().GetComPos().z - ls.leg.GetBaseLink().GetBody().GetComPos().z;
			ls.allow_late_stance_transition = ls.sagittal_pos < ls.leg_length * late_stance_threshold;
			ls.allow_liftoff_transition = ls.sagittal_pos < ls.leg_length * liftoff_threshold;
			ls.allow_landing_transition = ls.sagittal_pos > ls.leg_length * landing_threshold;
		}

		// update states
		for ( size_t idx = 0; idx < m_LegStates.size(); ++idx )
		{
			LegState& ls = *m_LegStates[ idx ];
			LegState& mir_ls = *m_LegStates[ idx ^ 1 ];
			LegState::GaitState new_state = ls.state;

			switch ( ls.state )
			{
			case LegState::UnknownState:
				// initialize state
				if ( ls.allow_stance_transition )
				{
					if ( mir_ls.allow_stance_transition && ls.sagittal_pos < mir_ls.sagittal_pos )
						new_state = LegState::LiftoffState;
					else if ( ls.allow_late_stance_transition )
						new_state = LegState::LateStanceState;
					else new_state = LegState::EarlyStanceState;
				}
				else
				{
					if ( ls.allow_landing_transition )
						new_state = LegState::LandingState;
					else new_state = LegState::SwingState;
				}
				break;

			case LegState::EarlyStanceState:
				// --> liftoff if other leg is eligible for stance and before this leg
				// --> late stance if a position threshold has passed
				if ( mir_ls.allow_stance_transition && ls.sagittal_pos < mir_ls.sagittal_pos )
					new_state = LegState::LiftoffState;
				else if ( ls.allow_late_stance_transition )
					new_state = LegState::LateStanceState;
				break;

			case LegState::LateStanceState:
				// --> liftoff if other leg is eligible for stance and before this leg
				// --> liftoff if position is beyond swing_threshold
				if ( mir_ls.allow_stance_transition && ls.sagittal_pos < mir_ls.sagittal_pos )
					new_state = LegState::LiftoffState;
				else if ( ls.allow_liftoff_transition )
					new_state = LegState::LiftoffState;
				break;

			case LegState::LiftoffState:
				// --> swing if leg load is below threshold
				if ( ls.allow_swing_transition )
					new_state = LegState::SwingState;
				break;

			case LegState::SwingState:
				// --> early stance if leg load is above threshold
				// --> landing if position is beyond landing_threshold
				if ( ls.allow_stance_transition && ls.sagittal_pos > mir_ls.sagittal_pos )
					new_state = LegState::EarlyStanceState;
				if ( !ls.allow_stance_transition && ls.allow_landing_transition )
					new_state = LegState::LandingState;
				break;

			case LegState::LandingState:
				// --> early stance if leg load is beyond threshold
				if ( ls.allow_stance_transition )
					new_state = LegState::EarlyStanceState;
				break;
			}

			if ( new_state != ls.state )
			{
				//log::TraceF( "%.3f: Leg %d state changed from %s to %s", timestamp, idx, ls.GetStateName().c_str(), LegState::m_StateNames.GetString( new_state ).c_str() );
				ls.state = new_state;
			}
		}
	}

	void GaitStateController::UpdateControllerStates( Model& model, double timestamp )
	{
		SCONE_PROFILE_FUNCTION;

		// update controller states
		for ( ConditionalControllerUP& cc : m_ConditionalControllers )
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
		for ( const ConditionalControllerUP& cc : m_ConditionalControllers )
			controllers[ cc->controller->GetSignature() ] += 1;

		// output number of controllers per leg
		for ( auto it = controllers.begin(); it != controllers.end(); ++it )
			s += "_" + to_str( it->second / m_LegStates.size() ) + it->first;

		return s;
	}

	void GaitStateController::StoreData( Storage<Real>::Frame& frame )
	{
		// store states
		for ( size_t idx = 0; idx < m_LegStates.size(); ++idx )
			frame[ m_LegStates[ idx ]->leg.GetName() + ".state" ] = m_LegStates[ idx ]->state;

		// store sagittal pos
		for ( size_t idx = 0; idx < m_LegStates.size(); ++idx )
			frame[ m_LegStates[ idx ]->leg.GetName() + ".sag_pos" ] = m_LegStates[ idx ]->sagittal_pos;

		for ( ConditionalControllerUP& cc : m_ConditionalControllers )
		{
			if ( cc->active )
				cc->controller->StoreData( frame );
		}
	}

	scone::String GaitStateController::GetConditionName( const ConditionalController& cc ) const
	{
		String s = m_LegStates[ cc.leg_index ]->leg.GetName();
		for ( int i = 0; i < LegState::StateCount; ++i )
		{
			if ( cc.state_mask.test( i ) )
				s += "-" + LegState::m_StateNames.GetString( LegState::GaitState( i ) );
		}
		return s;
	}
}
