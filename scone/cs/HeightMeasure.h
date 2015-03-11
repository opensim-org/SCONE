#pragma once

#include "Measure.h"
#include "../sim/Body.h"
#include "../core/MeasuredValue.h"

namespace scone
{
	namespace cs
	{
		class HeightMeasure : public Measure
		{
		public:
			HeightMeasure( const PropNode& props, opt::ParamSet& par, sim::Model& model );
			virtual ~HeightMeasure() { };

			virtual void UpdateControls( sim::Model& model, double timestamp ) override;
			virtual double GetResult( sim::Model& model ) override;

			virtual String GetSignature() override;

		private:
			String target_body;
			bool use_average_height;
			bool terminate_on_peak;
			double termination_height;

			sim::Body* m_pTargetBody; // non-owning pointer
			MeasuredValue< double > m_Height;
			bool m_Upward;
		};
	}
}
