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
		class CS_API Objective : public opt::Objective, public Propertyable
		{
		public:
			Objective();
			virtual ~Objective();

		private: // make class non-copyable by declaring copy-ctor and assignment private
			Objective( const Objective& );
			Objective& operator=( const Objective& );

		public:
			virtual opt::ParamSet GetParamSet() override;
			virtual double Evaluate( const opt::ParamSet& params ) override;

			virtual void ProcessPropNode(PropNode& props) override;

		private:
			PropNode m_SimulationProps;
			PropNode m_ControllerProps;
			PropNode m_MeasureProps;
		};
	}
}
