#pragma once
#include "Measure.h"
#include "../core/MeasuredValue.h"

namespace scone
{
	namespace cs
	{
		class GaitMeasure : public Measure
		{
		public:
			GaitMeasure( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area );
			virtual ~GaitMeasure();

			virtual void UpdateControls( sim::Model& model, double timestamp ) override;
			virtual double GetResult( sim::Model& model ) override;

			virtual String GetSignature() override;

			// parameters
			Real termination_height;
			Real min_velocity;
			Real duration;

		private:
			// settings
			std::vector< sim::Body* > m_GaitBodies;
			Real GetBackDist( sim::Model &model );

			Vec3 m_InitialComPos;
			MeasuredReal m_MinVelocityMeasure;
			Real m_InitBackDist;
		};
	}
}
