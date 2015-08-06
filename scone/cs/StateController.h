#pragma once

#include "cs.h"
#include "../sim/Controller.h"
#include "../core/PropNode.h"
#include "../opt/ParamSet.h"
#include "../sim/Model.h"

namespace scone
{
	namespace cs
	{
		class CS_API StateController : public sim::Controller
		{
		public:
			StateController( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area );
			virtual ~StateController();

			virtual UpdateResult UpdateControls( sim::Model& model, double timestamp ) override;

		protected:
			typedef size_t StateIndex;
			virtual size_t GetStateCount() = 0;
			virtual const String& GetStateName( StateIndex i ) = 0;

			struct ConditionalControllerState
			{
				std::vector< bool > state_mask;
				bool is_active;
				TimeInSeconds is_active_since;
			};

			typedef std::pair< ConditionalControllerState, sim::ControllerUP > ConditionalController;
			std::vector< ConditionalController > m_ConditionalControllers;

			void CreateConditionalControllers( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area );
			void UpdateConditionalControllerStates( StateIndex current_state, TimeInSeconds timestamp );

		private:
			StateController( const StateController& other );
			StateController& operator=( const StateController& other );

			virtual String GetClassSignature() const override;

		};
	}
}
