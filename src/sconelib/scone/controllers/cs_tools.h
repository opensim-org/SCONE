#pragma once

#include "scone/core/system_tools.h"
#include "scone/core/platform.h"
#include "scone/core/PropNode.h"
#include "scone/core/types.h"

// TODO: do this in a nicer way (i.e. push/pop warnings)
#pragma warning( disable: 4251 )

namespace scone
{
	PropNode SCONE_API RunSimulation( const path& filename, bool write_results_file = false );
	ModelObjectiveUP SCONE_API CreateModelObjective( const path& config_file );
}
