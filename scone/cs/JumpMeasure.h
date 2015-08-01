#pragma once

#include "cs.h"
#include "Measure.h"

#include "../core/PropNode.h"
#include "../opt/ParamSet.h"
#include "../sim/Model.h"

namespace scone
{
	namespace cs
	{
		class CS_API JumpMeasure : public Measure
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
			bool has_moved_up;
			Real distance;
			Real prev_force;
		};
	}
}
