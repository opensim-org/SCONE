#pragma once

#define USE_SHARK_V2 0

#include "scone/core/core.h"
#include "scone/core/PropNode.h"
#include "scone/core/string_tools.h"
#include "scone/core/memory_tools.h"
#include "scone/core/Log.h"

namespace scone
{
	namespace opt
	{
		// register factory types
		void SCONE_API RegisterFactoryTypes();

		// TODO: move this to sconeopt?
		void SCONE_API PerformOptimization( int argc, char* argv[] );
		void SCONE_API PerformOptimization( const String& scenario_file, const PropNode& props = PropNode("") );
		PropNode SCONE_API SimulateObjective( const String& par_file );
	}
}
