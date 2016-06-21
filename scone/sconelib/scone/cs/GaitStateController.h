#pragma once

#include "cs.h"
#include "scone/sim/Controller.h"
#include "scone/sim/Leg.h"
#include <bitset>
#include "scone/core/TimedValue.h"
#include "scone/core/StringMap.h"

namespace scone
{
	namespace cs
	{
		class SCONE_API GaitStateController : public sim::Controller
		{
		public:
			struct LegState
			{
				LegState( sim::Leg& l );

				// leg structure
				const sim::Leg& leg;
				sim::SensorDelayAdapter& load_sensor;

				// current state
				enum GaitState { UnknownState = -1, EarlyStanceState = 0, LateStanceState = 1, LiftoffState = 2, SwingState = 3, LandingState = 4, StateCount };
				const String& GetStateName() { return m_StateNames.GetString( state ); }
				static StringMap< GaitState > m_StateNames;
				TimedValue< GaitState > state;

				// current status
				Real leg_load;
				bool allow_stance_transition;
				bool allow_swing_transition;
				Real sagittal_pos;
				Real coronal_pos;
				bool allow_late_stance_transition;
				bool allow_liftoff_transition;
				bool allow_landing_transition;

				// cached constant state
				Real leg_length;
			};

			GaitStateController( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& target_area );
			virtual ~GaitStateController();

			virtual UpdateResult UpdateControls( sim::Model& model, double timestamp ) override;

			virtual String GetClassSignature() const override;

			// public parameters
			Real stance_load_threshold;
			Real swing_load_threshold;
		protected:
			virtual void UpdateLegStates( sim::Model& model, double timestamp );
			void UpdateControllerStates( sim::Model& model, double timestamp );

		private:
			typedef std::unique_ptr< LegState > LegStateUP;
			std::vector< LegStateUP > m_LegStates;

			// struct that defines if a controller is active (bitset denotes state(s), leg target should be part of controller)
			SCONE_DECLARE_STRUCT_AND_PTR( ConditionalController );
			struct ConditionalController
			{
				ConditionalController() : leg_index( NoIndex ), active( false ), active_since( 0.0 ) { };
				size_t leg_index;
				std::bitset< LegState::StateCount > state_mask;
				bool active;
				double active_since;
				sim::ControllerUP controller;
				String GetConditionName() const { return stringf( "L%dS%s", leg_index, state_mask.to_string().c_str() ); }
				bool TestLegPhase( size_t leg_idx, LegState::GaitState state ) { return state_mask.test( size_t( state ) ); }
			};
			String GetConditionName( const ConditionalController& cc ) const;
			std::vector< ConditionalControllerUP > m_ConditionalControllers;
			Real landing_threshold;
			Real late_stance_threshold;
			Real liftoff_threshold;
			Real override_leg_length;

			Real leg_load_sensor_delay;
			GaitStateController( const GaitStateController& );
			GaitStateController& operator=( const GaitStateController& );
		};
	}
}
