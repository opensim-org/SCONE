#pragma once
#include "Measure.h"

namespace scone
{
	namespace cs
	{
		class GaitMeasure : public Measure
		{
		public:
			GaitMeasure( const PropNode& props, opt::ParamSet& par, sim::Model& model );
			virtual ~GaitMeasure();

			virtual void UpdateControls( sim::Model& model, double timestamp ) override;
			virtual double GetResult( sim::Model& model ) override;

		private:
			// settings
			double termination_height;
			Vec3 m_InitialComPos;
		};
	}
}
