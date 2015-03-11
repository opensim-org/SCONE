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

			virtual String GetSignature() override;

		private:
			// settings
			std::vector< sim::Body* > m_GaitBodies;
			Real GetGaitDist( sim::Model& model );

			double termination_height;
			double m_InitialGaitDist;
			Vec3 m_InitialComPos;
		};
	}
}
