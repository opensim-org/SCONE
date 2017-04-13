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
		StateController( const StateController& other ) = delete;
		StateController& operator=( const StateController& other ) = delete;

		virtual UpdateResult UpdateControls( Model& model, double timestamp ) override;
		virtual void StoreData( Storage<Real>::Frame& frame ) override;

	protected:
		typedef size_t StateIndex;
		virtual size_t GetStateCount() const = 0;
		virtual const String& GetStateName( StateIndex i ) const = 0;
		virtual StateIndex GetCurrentState( Model& model, double timestamp ) = 0;

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
		void UpdateCurrentState( Model& model, TimeInSeconds timestamp );

	private:
		StateIndex m_CurrentState;
	};
}
