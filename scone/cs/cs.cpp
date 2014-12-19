#include "stdafx.h"
#include "cs.h"
#include "SimulationObjective.h"
#include "FeedForwardController.h"

namespace scone
{
	namespace cs
	{
		void CS_API RegisterFactoryTypes()
		{
			cs::SimulationObjective::RegisterFactory();
			cs::FeedForwardController::RegisterFactory();
		}
	}
}

