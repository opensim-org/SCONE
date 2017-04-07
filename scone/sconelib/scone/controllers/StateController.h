#pragma once

#include "scone/model/Controller.h"
#include "scone/core/PropNode.h"
#include "scone/optimization/ParamSet.h"
#include "scone/model/Model.h"

namespace scone
{
	class SCONE_API StateController : public Controller
	{
	public:
		StateController( const PropNode& props, ParamSet& par, Model& model, const Area& area );
		virtual ~StateController();

		virtual UpdateResult UpdateControls( Model& model, double timestamp ) override;

	protected:
		typedef size_t StateIndex;
		virtual size_t GetStateCount() = 0;
		virtual const String& GetStateName( StateIndex i ) = 0;
		virtual String GetClassSignature() const override;

		struct ConditionalControllerState
		{
			std::vector< bool > state_mask;
			bool is_active;
			TimeInSeconds is_active_since;
		};

		typedef std::pair< ConditionalControllerState, ControllerUP > ConditionalController;
		std::vector< ConditionalController > m_ConditionalControllers;

		void CreateConditionalControllers( const PropNode& props, ParamSet& par, Model& model, const Area& area );
		void UpdateConditionalControllerStates( StateIndex current_state, TimeInSeconds timestamp );

	private:
		StateController( const StateController& other );
		StateController& operator=( const StateController& other );
	};
}
