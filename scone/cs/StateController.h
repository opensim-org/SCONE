#pragma once

#include "cs.h"
#include "../sim/Controller.h"
#include "../sim/Leg.h"
#include <bitset>
#include "../core/TimedValue.h"

namespace scone
{
	namespace cs
	{
		class CS_API StateController : public sim::Controller
		{
		public:
			struct LegState
			{
				LegState( const sim::Leg& l );
				const sim::Leg& leg;

				// current state
				enum State { UnknownState = -1, StanceState, LiftoffState, SwingState, LandingState, StateCount };
				TimedValue< State > state;

				// current status
				bool contact;
				Real sagittal_pos;
				Real coronal_pos;
			};

			StateController( const PropNode& props, opt::ParamSet& par, sim::Model& model );
			virtual ~StateController();

			virtual void UpdateControls( sim::Model& model, double timestamp ) override;

			// public parameters
			Real contact_force_threshold;
			Real saggital_anterior_threshold;
			Real saggital_posterior_threshold;

		protected:
			virtual void UpdateLegStates( sim::Model& model, double timestamp );
			void UpdateControllerStates( sim::Model& model, double timestamp );

		private:
			typedef std::unique_ptr< LegState > LegStateUP;
			std::vector< LegStateUP > m_LegStates;

			// struct that defines if a controller is active (vector denotes leg, bitset denotes state(s))
			struct ControllerState
			{
				std::vector< std::bitset< LegState::StateCount > > leg_condition;
				bool active;
				double active_since;
			};
			std::vector< std::pair< ControllerState, sim::ControllerUP > > m_Controllers;

			StateController( const StateController& );
			StateController& operator=( const StateController& );
		};
	}
}
