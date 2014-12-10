#pragma once

#include "cs.h"
#include "Measure.h"

#include "..\sim\sim.h"
#include "..\opt\Objective.h"
#include "..\opt\ParamSet.h"
#include "..\sim\Simulation.h"
#include "..\core\PropNode.h"
#include <vector>
#include "ParameterizableController.h"

namespace scone
{
	namespace cs
	{
		class CS_API SimulationObjective : public opt::Objective
		{
		public:
			SimulationObjective();
			virtual ~SimulationObjective();

			virtual double Evaluate() override;
			virtual void ProcessProperties( const PropNode& props ) override;
			virtual void ProcessParameters( opt::ParamSet& par ) override;

		private:
			PropNode m_Props;
			opt::ParamSet m_Params;
		};
	}
}
