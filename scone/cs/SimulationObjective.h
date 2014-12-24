#pragma once

#include "cs.h"
#include "..\sim\sim.h"
#include "..\opt\Objective.h"
#include "..\opt\ParamSet.h"
#include "..\sim\Simulation.h"
#include "..\core\PropNode.h"

#include "Measure.h"

#include <vector>

namespace scone
{
	namespace cs
	{
		class CS_API SimulationObjective : public opt::Objective, public Factoryable< opt::Objective, SimulationObjective >
		{
		public:
			SimulationObjective();
			virtual ~SimulationObjective();

			virtual double Evaluate() override;
			virtual void ProcessProperties( const PropNode& props ) override;
			virtual void ProcessParameters( opt::ParamSet& par ) override;

		private:
			double max_duration;
			PropNode m_Props;
			opt::ParamSet m_Params;
		};
	}
}
