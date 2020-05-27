#pragma once

#include "platform.h"
#include "xo/time/time.h"
#include "PropNode.h"
#include "xo/filesystem/path.h"
#include "types.h"

namespace scone
{
	/// Creates and evaluates SimulationObjective. Logs unused properties.
	SCONE_API void BenchmarkScenario( const PropNode& scenario_pn, const xo::path& file, size_t evals );

	struct SCONE_API Benchmark {
		String name_;
		xo::time time_;
		xo::time baseline_;
		double std_;

		xo::time diff() const { return time_ - baseline_; }
		double diff_perc() const { return 100 * ( diff() / baseline_ ); }
		double diff_std() const { return diff().nanosecondsd() / std_; }
	};
}
