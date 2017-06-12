#pragma once

//#include "cs_fwd.h"
#include "scone/optimization/Objective.h"
#include "scone/optimization/Params.h"
#include "scone/model/Simulation.h"
#include "scone/core/PropNode.h"

#include "Measure.h"
#include <vector>

namespace scone
{
	class SCONE_API SimulationObjective : public Objective
	{
	public:
		SimulationObjective( const PropNode& props );
		virtual ~SimulationObjective();

		virtual std::vector< String > WriteResults( const String& file ) override;

		Model& GetModel() { return *m_Model; }
		Measure& GetMeasure() { return *m_Measure; }

		double max_duration;

		virtual fitness_t evaluate( const ParamInstance& point ) const override;

		void CreateModelFromParameters( Params& par );

	protected:
		virtual String GetClassSignature() const override;

	private:
		ModelUP m_Model;
		Measure* m_Measure;
		PropNode m_ModelProps;
	};
}
