#pragma once

#include "scone/optimization/Objective.h"
#include "scone/optimization/Params.h"
#include "scone/model/Simulation.h"
#include "scone/core/PropNode.h"

#include "Measure.h"
#include <vector>
#include "flut/system/path.hpp"
#include "ModelObjective.h"

namespace scone
{
	class SCONE_API SimulationObjective : public ModelObjective
	{
	public:
		SimulationObjective( const PropNode& props );
		virtual ~SimulationObjective();

		double max_duration;

		virtual fitness_t EvaluateModel( Model& m ) const override;
		virtual TimeInSeconds GetDuration() const override { return max_duration; }

		virtual void AdvanceModel( Model& m, TimeInSeconds t ) const override;
		virtual fitness_t GetResult( Model& m ) const override { return m.GetMeasure()->GetResult( m ); }
		virtual PropNode GetReport( Model& m ) const override { return m.GetMeasure()->GetReport(); }

	protected:
		virtual String GetClassSignature() const override;

	private:
		String m_Signature;
	};
}
