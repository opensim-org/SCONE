#pragma once

#include "scone/optimization/Objective.h"
#include "scone/optimization/Params.h"
#include "scone/model/Simulation.h"
#include "scone/core/PropNode.h"

#include "scone/measures/Measure.h"
#include <vector>
#include "xo/filesystem/path.h"
#include "ModelObjective.h"

namespace scone
{
	/// Objective in which a simulation is run.
	class SCONE_API SimulationObjective : public ModelObjective
	{
	public:
		SimulationObjective( const PropNode& props );
		virtual ~SimulationObjective();

		/// Maximum duration after which the evaluation is terminated; default = 1e12 (+/-31000 years)
		double max_duration;

		/// Measure to be used for the Objective.
		PropNode measure;

		virtual fitness_t EvaluateModel( Model& m ) const override;
		virtual TimeInSeconds GetDuration() const override { return max_duration; }

		virtual void AdvanceModel( Model& m, TimeInSeconds t ) const override;
		virtual fitness_t GetResult( Model& m ) const override { return m.GetMeasure()->GetResult( m ); }
		virtual PropNode GetReport( Model& m ) const override { return m.GetMeasure()->GetReport(); }

		virtual ModelUP CreateModelFromParams( Params& point ) const override;

	protected:
		virtual String GetClassSignature() const override;

	private:
		String signature_;
		std::vector< path > external_files_;
	};
}
