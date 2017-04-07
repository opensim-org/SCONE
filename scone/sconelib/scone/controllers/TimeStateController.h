#pragma once

#include "StateController.h"

namespace scone
{
	class SCONE_API TimeStateController : public StateController
	{
	public:
		TimeStateController( const PropNode& props, ParamSet& par, Model& model, const Area& area );
		virtual ~TimeStateController();

		virtual size_t GetStateCount() override { return m_States.size(); }
		virtual const String& GetStateName( StateIndex i ) override { return m_States[ i ].name; }

		virtual UpdateResult UpdateAnalysis( const Model& model, double timestamp ) override;

	protected:
		void UpdateCurrentState( double timestamp );
		virtual String GetClassSignature() const override;

		struct TimeState
		{
			TimeState( const PropNode& pn, ParamSet& par );
			bool operator<( const TimeState& other ) const { return start_time < other.start_time; }
			String name;
			TimeInSeconds start_time;
		};

		std::vector< TimeState > m_States;
		StateIndex m_CurrentState;
	};
}
