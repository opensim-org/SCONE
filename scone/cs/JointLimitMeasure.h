#pragma once

#include "Measure.h"

namespace scone
{
	namespace cs
	{
		class JointLimitMeasure : public Measure
		{
		public:
			JointLimitMeasure( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area );
			virtual ~JointLimitMeasure();

			virtual void UpdateControls( sim::Model& model, double timestamp ) override;
			virtual double GetResult( sim::Model& model ) override;

		private:
			virtual String GetSignature() override;
		};
	}
}
