#pragma once

#include "../core/core.h"
#include "../sim/sim.h"
#include "../core/PropNode.h"

namespace scone
{
	class SconeManager
	{
	public:
		SconeManager();
		virtual ~SconeManager();

		void SimulateObjective( const String& filename );
		const PropNode& GetSettings();

	private:
		PropNode m_Settings;
		PropNode m_Statistics;
	};
}
