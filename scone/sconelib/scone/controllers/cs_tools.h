#pragma once

#include "scone/core/system_tools.h"
#include "scone/core/core.h"
#include "scone/core/PropNode.h"
#include "scone/controllers/cs_fwd.h"

// TODO: do this in a nicer way (i.e. push/pop warnings)
#pragma warning( disable: 4251 )

namespace scone
{
	PropNode SCONE_API RunSimulation( const path& filename, bool write_results_file = false );
	SimulationObjectiveUP SCONE_API CreateSimulationObjective( const path& filename );
}
