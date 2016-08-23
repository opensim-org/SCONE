#pragma once

#define USE_SHARK_V2 0

#include "scone/core/core.h"
#include "scone/core/PropNode.h"
#include "scone/core/string_tools.h"
#include "scone/core/memory_tools.h"
#include "scone/core/Log.h"
#include "scone/opt/Optimizer.h"

namespace scone
{
	namespace opt
	{
		// register factory types
		SCONE_API void RegisterFactoryTypes();

		// TODO: move this to sconeopt?
		SCONE_API OptimizerUP PrepareOptimization( const PropNode& props, const String& scenario_file );
		SCONE_API PropNode SimulateObjective( const String& par_file );
	}
}
