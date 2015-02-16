#pragma once
#include "Measure.h"

namespace scone
{
	namespace cs
	{
		class GaitMeasure : public Measure, public Factoryable< sim::Controller, GaitMeasure >
		{
		public:
			GaitMeasure( const PropNode& props );
			virtual ~GaitMeasure();

			virtual void Initialize( sim::Model& model, opt::ParamSet& par, const PropNode& props ) override;
			virtual void UpdateControls( sim::Model& model, double timestamp ) override;
			virtual double GetResult( sim::Model& model ) override;

		private:
			// settings
			double termination_height;

			Vec3 m_InitialComPos;

		};
	}
}
