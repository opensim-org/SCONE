#pragma once

#include "scone/core/core.h"
#include "scone/core/PropNode.h"

namespace scone
{
	class SconeManager
	{
	public:
		SconeManager();
		virtual ~SconeManager();

		void SimulateObjective( const String& filename );

	private:
		PropNode m_Settings;
		PropNode m_Statistics;
	};
}
