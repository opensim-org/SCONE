#pragma once

#include "Measure.h"
#include "../sim/Body.h"
#include "../core/Statistic.h"

namespace scone
{
	namespace cs
	{
		class HeightMeasure : public Measure
		{
		public:
			HeightMeasure( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area );
			virtual ~HeightMeasure() { };

			virtual UpdateResult UpdateAnalysis( const sim::Model& model, double timestamp ) override;
			virtual double GetResult( sim::Model& model ) override;

		protected:
			virtual String GetClassSignature() const override;

		private:
			String target_body;
			bool use_average_height;
			bool terminate_on_peak;
			double termination_height;

			sim::Body* m_pTargetBody; // non-owning pointer
			Statistic< double > m_Height;
			bool m_Upward;
		};
	}
}
