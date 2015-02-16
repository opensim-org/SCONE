#pragma once

#include "Measure.h"
#include "../sim/Body.h"
#include <functional>
#include "../core/MeasuredValue.h"

namespace scone
{
	namespace cs
	{
		class HeightMeasure : public Measure
		{
		public:
			HeightMeasure( const PropNode& props );
			virtual ~HeightMeasure() { };

			virtual void Initialize( sim::Model& model, opt::ParamSet& par, const PropNode& props ) override;
			virtual void UpdateControls( sim::Model& model, double timestamp ) override;
			virtual double GetResult( sim::Model& model ) override;

		private:
			String target_body;
			bool use_average_height;
			bool terminate_on_peak;
			double termination_height;

			sim::Body* m_pTargetBody; // non-owning pointer
			MeasuredValue< double > m_Height;
			bool m_Upward;
			size_t m_LastStep;
		};
	}
}
