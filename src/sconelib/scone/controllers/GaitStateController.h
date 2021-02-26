/*
** GaitStateController.h
**
** Copyright (C) 2013-2021 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "scone/core/types.h"
#include "scone/controllers/Controller.h"
#include "scone/model/Leg.h"
#include <bitset>
#include "scone/core/TimedValue.h"
#include "scone/core/StringMap.h"
#include "scone/core/string_tools.h"

namespace scone
{
	/// Finite State Machine controller that detects different phases of gait.
	/**
	Supported states are: ''EarlyStance'', ''LateStance'', ''LiftOff'', ''Swing'', ''Landing''.

	The current state of a leg is defined through its current relative sagittal foot position (''sagittal_pos'') and the normalized load (''leg_load'').
	Transitions between states are defined as follows (see also public attributes below):
	* ''Landing'' -> ''EarlyStance'' occurs if ''leg_load'' is above **stance_load_threshold**.
	* ''EarlyStance'' -> ''LateStance'' occurs if ''sagittal_pos'' is below **late_stance_threshold**.
	* ''LateStance'' -> ''LiftOff'' occurs if contralateral ''leg_load'' is above **stance_load_threshold** or ''sagittal_pos'' is below **liftoff_threshold**.
	* ''LiftOff'' -> ''Swing'' occurs if ''leg_load'' is below **swing_load_threshold**.
	* ''Swing'' -> ''Landing'' occurs if ''sagittal_pos'' is above **landing_threshold**.

	See the [[https://scone.software/doku.php?id=tutorials:gait | gait tutorial]] for an example.
	*/
	class GaitStateController : public Controller
	{
	public:
		GaitStateController( const PropNode& props, Params& par, Model& model, const Location& target_area );
		virtual ~GaitStateController();
		GaitStateController( const GaitStateController& ) = delete;
		GaitStateController& operator=( const GaitStateController& ) = delete;

		/// Gait states that can be detected by the controller
		enum GaitState { UnknownState = -1, EarlyStanceState = 0, LateStanceState = 1, LiftoffState = 2, SwingState = 3, LandingState = 4, StateCount };

		/// Relative sagittal distance [leg length] of the swing foot used for detecting LandingState; default = 0.
		Real landing_threshold;

		/// Relative sagittal distance [leg length] of the stance foot used for detecting LateStanceState; default = 0.
		Real late_stance_threshold;

		/// Relative sagittal distance [leg length] of the stance foot used for detecting LiftoffState; default = -1.
		Real liftoff_threshold;

		/// If defined, use custom leg length instead of deriving from model; default = [not set].
		Real override_leg_length;

		/// Neural delay [s] used for load sensors; default = 0.
		Real leg_load_sensor_delay;

		/// Threshold load [bw] for detecting stance phase; default = 0.
		Real stance_load_threshold;

		/// Threshold load [bw] for detecting swing phase; default = stance_load_threshold.
		Real swing_load_threshold;

		/// Bool indicating if control parameters should be the same for left and right; default = true.
		bool symmetric;

		virtual bool ComputeControls( Model& model, double timestamp ) override;
		virtual String GetClassSignature() const override;
		virtual void StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags ) const override;

	protected:
		struct LegState
		{
			LegState( Model& m, Leg& l, const PropNode& props, Params& par );

			// leg structure
			const Leg& leg;
			SensorDelayAdapter& load_sensor;

			// current state
			const String& GetStateName() { return m_StateNames.GetString( state ); }
			TimedValue< GaitState > state;

			// current status
			Real leg_load;
			Real sagittal_pos;
			Real coronal_pos;
			bool allow_stance_transition;
			bool allow_swing_transition;
			bool allow_late_stance_transition;
			bool allow_liftoff_transition;
			bool allow_landing_transition;

			// cached constants
			const Real leg_length;
			const Real stance_load_threshold;
			const Real swing_load_threshold;
			const Real landing_threshold;
			const Real late_stance_threshold;
			const Real liftoff_threshold; // default value is such that parameter has no effect
		};

		virtual void UpdateLegStates( Model& model, double timestamp );
		void UpdateControllerStates( Model& model, double timestamp );

		static StringMap< GaitState > m_StateNames;

	private:
		typedef std::unique_ptr< LegState > LegStateUP;
		std::vector< LegStateUP > m_LegStates;

		// struct that defines if a controller is active (bitset denotes state(s), leg target should be part of controller)
		SCONE_DECLARE_STRUCT_AND_PTR( ConditionalController );
		struct ConditionalController
		{
			ConditionalController() : leg_index( NoIndex ), active( false ), active_since( 0.0 ) { };
			size_t leg_index;
			std::bitset< StateCount > state_mask;
			bool active;
			double active_since;
			ControllerUP controller;
			String GetConditionName() const { return stringf( "L%dS%s", leg_index, state_mask.to_string().c_str() ); }
			bool TestLegPhase( size_t leg_idx, GaitState state ) { return state_mask.test( size_t( state ) ); }
		};
		String GetConditionName( const ConditionalController& cc ) const;
		std::vector< ConditionalControllerUP > m_ConditionalControllers;
	};
}
