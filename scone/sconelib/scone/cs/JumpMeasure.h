#pragma once

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
			enum State { Prepraration, Takeoff, Flight, Landing };
			double GetHighJumpResult( const sim::Model& m );
			double GetLongJumpResult( const sim::Model& m );

			State state;
			sim::Body* target_body;
			Real termination_height;
			bool terminate_on_peak;
			Vec3 init_com;
			Vec3 prepare_com;
			Vec3 jump_com;
			Real prepare_time;
			bool negate_result;
			enum JumpType { NoJumpType, HighJump, LongJump };
			int jump_type;
		};
	}
}
