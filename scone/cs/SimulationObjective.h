#pragma once

#include "cs.h"
#include "../sim/sim.h"
#include "../opt/Objective.h"
#include "../opt/ParamSet.h"
#include "../sim/Simulation.h"
#include "../core/PropNode.h"

#include "Measure.h"

#include <vector>

namespace scone
{
	namespace cs
	{
		class CS_API SimulationObjective : public opt::Objective
		{
		public:
			SimulationObjective( const PropNode& props );
			virtual ~SimulationObjective();

			virtual double Evaluate() override;
			virtual void ProcessParameters( opt::ParamSet& par ) override;

			virtual void WriteResults( const String& file ) override;

		private:
			double max_duration;
			sim::ModelUP m_Model;
		};
	}
}
