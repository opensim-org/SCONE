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
			struct LegState
			{
				LegState( const sim::Leg& l ) : leg( l ), phase( UnknownState ), contact( false ), sagittal_pos( 0.0 ), coronal_pos( 0.0 ) {};
				const sim::Leg& leg;

				// current state
				enum Phase { UnknownState = -1, StanceState = 0, LiftoffState = 1, SwingState = 2, LandingState = 3, StateCount };
				const String& GetPhaseName() { return m_PhaseNames.GetString( phase ); }
				static EnumStringMap< Phase > m_PhaseNames;
				TimedValue< Phase > phase;

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
			typedef std::unique_ptr< LegState > LegStateUP;
			std::vector< LegStateUP > m_LegStates;

			// struct that defines if a controller is active (bitset denotes state(s), leg target should be part of controller)
			SCONE_DECLARE_CLASS_AND_PTR( ConditionalController );
			class ConditionalController
			{
			public:
				ConditionalController( const PropNode& props, opt::ParamSet& par, sim::Model& model, const PropNode& mask );
				virtual ~ConditionalController() {}
				size_t leg_index;
				std::bitset< LegState::StateCount > phase_mask;
				bool active;
				double active_since;
				sim::ControllerUP controller;
				bool TestLegPhase( size_t leg_idx, LegState::Phase state ) { return phase_mask.test( size_t( state ) ); }
			};
			std::vector< ConditionalControllerUP > m_ConditionalControllers;
			Real landing_threshold;
			GaitStateController( const GaitStateController& );
			GaitStateController& operator=( const GaitStateController& );
		};
	}
}
