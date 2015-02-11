#pragma once

#include "cs.h"
#include "../sim/Controller.h"
#include "../core/Factory.h"
#include "../sim/Leg.h"
#include <bitset>
#include "../core/TimedValue.h"

namespace scone
{
	namespace cs
	{
		class CS_API StateController : public sim::Controller, public Factoryable< sim::Controller, StateController >
		{
		public:
			struct LegState
			{
				LegState( const sim::Leg& l ) : leg( l ), state( UnknownState ) {};
				const sim::Leg& leg;
				enum State { UnknownState = -1, StanceState, LiftoffState, SwingState, LandingState, StateCount };
				TimedValue< State > state;
			};

			StateController( const PropNode& props );
			virtual ~StateController();

			virtual void Initialize( sim::Model& model ) override;
			virtual void UpdateControls( sim::Model& model, double timestamp ) override;
			virtual void ProcessParameters( opt::ParamSet& par ) override;

			// public parameters
			double contact_force_threshold;

		protected:
			virtual void UpdateLegStates( double timestamp );
			void UpdateControllerStates( double timestamp );

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
