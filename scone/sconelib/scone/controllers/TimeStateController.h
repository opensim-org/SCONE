#pragma once

#include "StateController.h"

namespace scone
{
	class SCONE_API TimeStateController : public StateController
	{
	public:
		TimeStateController( const PropNode& props, ParamSet& par, Model& model, const Area& area );

		virtual size_t GetStateCount() const override { return m_States.size(); }
		virtual const String& GetStateName( StateIndex i ) const override { return m_States[ i ].name; }

	protected:
		virtual String GetClassSignature() const override;
		virtual StateIndex GetCurrentState( Model& model, double timestamp ) override;

		struct TimeState
		{
			TimeState( const PropNode& pn, ParamSet& par );
			bool operator<( const TimeState& other ) const { return start_time < other.start_time; }
			String name;
			TimeInSeconds start_time;
		};

		std::vector< TimeState > m_States;
	};
}
