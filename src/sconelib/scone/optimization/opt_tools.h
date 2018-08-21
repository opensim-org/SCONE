#pragma once

#include "scone/core/platform.h"
#include "scone/core/PropNode.h"
#include "scone/core/string_tools.h"
#include "scone/core/memory_tools.h"
#include "scone/core/Log.h"
#include "scone/core/types.h"
#include "scone/optimization/Optimizer.h"

namespace scone
{
	// TODO: move this to sconeopt?
	SCONE_API OptimizerUP PrepareOptimization( const PropNode& props, const path& scenario_file );
	SCONE_API PropNode SimulateObjective( const path& par_file );
}
