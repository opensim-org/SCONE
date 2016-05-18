#pragma once

#include "Measure.h"
#include "scone/core/Statistic.h"
#include "../sim/Joint.h"
#include "RangePenalty.h"

namespace scone
{
	namespace cs
	{
		class SCONE_API JointLoadMeasure : public Measure
		{
		public:
			JointLoadMeasure( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area );
			virtual ~JointLoadMeasure() {}

			enum Method { NoMethod, JointReactionForce };

			virtual double GetResult( sim::Model& model ) override;
			virtual UpdateResult UpdateAnalysis( const sim::Model& model, double timestamp ) override;

		protected:
			virtual String GetClassSignature() const override;
			virtual void StoreData( Storage< Real >::Frame& frame ) override;

		private:
			int method;
			Real joint_load;
			RangePenalty< Real > load_penalty;
			sim::Joint& joint;
		};
	}
}
