#pragma once

#include "cs.h"
#include "scone/sim/sim.h"
#include "scone/opt/Objective.h"
#include "scone/opt/ParamSet.h"
#include "scone/sim/Simulation.h"
#include "scone/core/PropNode.h"

#include "Measure.h"

#include <vector>

namespace scone
{
	namespace cs
	{
		class CS_API SimulationObjective : public opt::Objective
		{
		public:
			SimulationObjective( const PropNode& props, opt::ParamSet& par );
			virtual ~SimulationObjective();

			virtual double Evaluate() override;

			virtual std::vector< String > WriteResults( const String& file ) override;

			sim::Model& GetModel() { return *m_Model; }
			cs::Measure& GetMeasure() { return *m_Measure; }

			double max_duration;

		protected:
			virtual String GetClassSignature() const override;
			virtual void ProcessParameters( opt::ParamSet& par ) override;

		private:
			sim::ModelUP m_Model;
			cs::Measure* m_Measure;
			const PropNode& m_ModelProps;
		};
	}
}
