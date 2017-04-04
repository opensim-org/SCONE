#pragma once

//#include "cs_fwd.h"
#include "scone/optimization/Objective.h"
#include "scone/optimization/ParamSet.h"
#include "scone/model/Simulation.h"
#include "scone/core/PropNode.h"

#include "Measure.h"

#include <vector>

namespace scone
{
	class SCONE_API SimulationObjective : public opt::Objective
	{
	public:
		SimulationObjective( const PropNode& props, opt::ParamSet& par );
		virtual ~SimulationObjective();

		virtual double Evaluate() override;

		virtual std::vector< String > WriteResults( const String& file ) override;

		sim::Model& GetModel() { return *m_Model; }
		Measure& GetMeasure() { return *m_Measure; }

		double max_duration;

	protected:
		virtual String GetClassSignature() const override;
		virtual void ProcessParameters( opt::ParamSet& par ) override;

	private:
		sim::ModelUP m_Model;
		Measure* m_Measure;
		const PropNode& m_ModelProps;
	};
}
