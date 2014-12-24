#include "stdafx.h"
#include "cs.h"
#include "SimulationObjective.h"
#include "FeedForwardController.h"

#include <OpenSim/OpenSim.h>
#include "../sim/simbody/sim_simbody.h"
#include "JumpingMeasure.h"

namespace scone
{
	namespace cs
	{
		void CS_API RegisterFactoryTypes()
		{
			// simulation engines
			sim::RegisterSimbody();

			// cs types
			SimulationObjective::RegisterFactory();
			FeedForwardController::RegisterFactory();
			JumpingMeasure::RegisterFactory();
		}
	}
}
