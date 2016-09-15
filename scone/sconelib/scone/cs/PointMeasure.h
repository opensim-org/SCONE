#pragma once

#include "Measure.h"
#include "scone/sim/Body.h"
#include "scone/core/Range.h"
#include "scone/core/Statistic.h"

namespace scone
{
	namespace cs
	{
		class PointMeasure : public Measure
		{
		public:
			PointMeasure( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area );
			virtual ~PointMeasure() { };

			virtual UpdateResult UpdateAnalysis( const sim::Model& model, double timestamp ) override;
			virtual double GetResult( sim::Model& model ) override;

		protected:
			virtual String GetClassSignature() const override;
			virtual void StoreData( Storage< Real >::Frame& frame ) override;

		private:
			String body;
			Vec3 offset;
			Vec3 axes_to_measure;
			bool relative_to_model_com;

			Range< Real > range;
			Range< Real > velocity_range;
			Real squared_range_penalty;
			Real abs_range_penalty;
			Real squared_velocity_range_penalty;
			Real abs_velocity_range_penalty;
			Statistic<> penalty;

			sim::Body* m_pTargetBody; // non-owning pointer
		};
	}
}
