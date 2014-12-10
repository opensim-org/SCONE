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

		private: // make class non-copyable by declaring copy-ctor and assignment private
			SimulationObjective( const SimulationObjective& );
			SimulationObjective& operator=( const SimulationObjective& );

		public:
			virtual double Evaluate() override;
			virtual void ProcessProperties(const PropNode& props) override;
			virtual void ProcessParameters( opt::ParamSet& par ) override;

		private:
			PropNode m_SimulationProps;
			PropNode m_ControllerProps;
			PropNode m_MeasureProps;
		};
	}
}
