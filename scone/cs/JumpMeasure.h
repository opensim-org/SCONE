#pragma once

#include "cs.h"
#include "Measure.h"

#include "scone/core/PropNode.h"
#include "scone/opt/ParamSet.h"
#include "scone/sim/Model.h"

namespace scone
{
	namespace cs
	{
		class SCONE_API JumpMeasure : public Measure
		{
		public:
			JumpMeasure( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area );
			virtual ~JumpMeasure();

			virtual double GetResult( sim::Model& model ) override;
			virtual UpdateResult UpdateAnalysis( const sim::Model& model, double timestamp ) override;
			virtual String GetClassSignature() const override;

		private:
			sim::Body& target_body;
			Real termination_height;
			Real init_dist;
			Real init_height;
			bool was_airborne;
			Real distance;
			Real prev_force;
			Real ignore_time;
		};
	}
}
