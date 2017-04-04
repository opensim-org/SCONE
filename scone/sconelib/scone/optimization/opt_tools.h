#pragma once

#include "scone/core/core.h"
#include "scone/core/PropNode.h"
#include "scone/core/string_tools.h"
#include "scone/core/memory_tools.h"
#include "scone/core/Log.h"
#include "scone/optimization/Optimizer.h"

namespace scone
{
	namespace opt
	{
		// TODO: move this to sconeopt?
		SCONE_API OptimizerUP PrepareOptimization( const PropNode& props, const path& scenario_file );
		SCONE_API PropNode SimulateObjective( const path& par_file );
	}
}
