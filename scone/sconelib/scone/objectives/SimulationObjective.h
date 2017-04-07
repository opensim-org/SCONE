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
	class SCONE_API SimulationObjective : public Objective
	{
	public:
		SimulationObjective( const PropNode& props, ParamSet& par );
		virtual ~SimulationObjective();

		virtual double Evaluate() override;

		virtual std::vector< String > WriteResults( const String& file ) override;

		Model& GetModel() { return *m_Model; }
		Measure& GetMeasure() { return *m_Measure; }

		double max_duration;

	protected:
		virtual String GetClassSignature() const override;
		virtual void ProcessParameters( ParamSet& par ) override;

	private:
		ModelUP m_Model;
		Measure* m_Measure;
		const PropNode& m_ModelProps;
	};
}
