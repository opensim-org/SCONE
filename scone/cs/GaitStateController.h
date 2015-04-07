#pragma once

#include "cs.h"
#include "../sim/Controller.h"
#include "../sim/Leg.h"
#include <bitset>
#include "../core/TimedValue.h"
#include "../core/EnumStringMap.h"

namespace scone
{
	namespace cs
	{
		class CS_API GaitStateController : public sim::Controller
		{
		public:
			struct LegInfo
			{
				LegInfo( const sim::Leg& l ) : leg( l ), state( UnknownState ), contact( false ), sagittal_pos( 0.0 ), coronal_pos( 0.0 ) {};
				const sim::Leg& leg;

				// current state
				enum GaitState { UnknownState = -1, EarlyStanceState = 0, LateStanceState = 1, LiftoffState = 2, SwingState = 3, LandingState = 4, StateCount };
				const String& GetStateName() { return m_StateNames.GetString( state ); }
				static EnumStringMap< GaitState > m_StateNames;
				TimedValue< GaitState > state;

				// current status
				bool contact;
				Real sagittal_pos;
				Real coronal_pos;
			};

			GaitStateController( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& target_area );
			virtual ~GaitStateController();

			virtual void UpdateControls( sim::Model& model, double timestamp ) override;

			virtual String GetSignature() override;

			// public parameters
			Real contact_force_threshold;
			Real saggital_anterior_threshold;
			Real saggital_posterior_threshold;

		protected:
			virtual void UpdateLegStates( sim::Model& model, double timestamp );
			void UpdateControllerStates( sim::Model& model, double timestamp );

		private:
			typedef std::unique_ptr< LegInfo > LegStateUP;
			std::vector< LegStateUP > m_LegStates;

			// struct that defines if a controller is active (bitset denotes state(s), leg target should be part of controller)
			SCONE_DECLARE_CLASS_AND_PTR( ConditionalController );
			class ConditionalController
			{
			public:
				ConditionalController( const PropNode& props, opt::ParamSet& par, sim::Model& model, const PropNode& mask );
				virtual ~ConditionalController() {}
				size_t leg_index;
				std::bitset< LegInfo::StateCount > state_mask;
				bool active;
				double active_since;
				sim::ControllerUP controller;
				bool TestLegPhase( size_t leg_idx, LegInfo::GaitState state ) { return state_mask.test( size_t( state ) ); }
			};
			std::vector< ConditionalControllerUP > m_ConditionalControllers;
			Real landing_threshold;
			Real late_stance_threshold;
			GaitStateController( const GaitStateController& );
			GaitStateController& operator=( const GaitStateController& );
		};
	}
}
