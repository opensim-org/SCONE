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
	SCONE_API OptimizerUP PrepareOptimization( const PropNode& scenario_pn, const path& scenario_file );
	SCONE_API PropNode EvaluateScenario( const PropNode& scenario_pn, const path& par_file, const path& output_base, double data_resulotion = 0.001 );
	SCONE_API path FindScenario( const path& scenario_or_par_file );
}
