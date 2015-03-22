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
				LegState( const sim::Leg& l ) : leg( l ), state( UnknownState ), contact( false ), sagittal_pos( 0.0 ), coronal_pos( 0.0 ) {};
				const sim::Leg& leg;

				// current state
				enum State { UnknownState = -1, StanceState = 0, LiftoffState = 1, SwingState = 2, LandingState = 3, StateCount };
				static const char* state_names[];
				TimedValue< State > state;

				// current status
				bool contact;
				Real sagittal_pos;
				Real coronal_pos;
			};

			StateController( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& target_area );
			virtual ~StateController();

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
			typedef std::unique_ptr< LegState > LegStateUP;
			std::vector< LegStateUP > m_LegStates;

			// struct that defines if a controller is active (bitset denotes state(s), leg target should be part of controller)
			SCONE_DECLARE_CLASS_AND_PTR( ConditionalController );
			class ConditionalController
			{
			public:
				ConditionalController( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Leg& leg );
				virtual ~ConditionalController() {}
				size_t leg_index;
				std::bitset< LegState::StateCount > state_mask;
				bool active;
				double active_since;
				sim::ControllerUP controller;
				bool TestLegState( size_t leg_idx, LegState::State state ) { return state_mask.test( size_t( state ) ); }
			};
			std::vector< ConditionalControllerUP > m_ConditionalControllers;
			Real landing_offset;
			StateController( const StateController& );
			StateController& operator=( const StateController& );
		};
	}
}
