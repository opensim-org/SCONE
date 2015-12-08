#pragma once

#include "Measure.h"
#include "../core/InitFromPropNode.h"
#include "../core/Range.h"
#include "../sim/Dof.h"
#include "../core/Statistic.h"

namespace scone
{
	namespace cs
	{
		class DofLimitMeasure : public Measure
		{
		public:
			DofLimitMeasure( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area );
			virtual ~DofLimitMeasure();

			virtual UpdateResult UpdateAnalysis( const sim::Model& model, double timestamp ) override;
			virtual double GetResult( sim::Model& model ) override;
			virtual PropNode GetReport();

		protected:
			virtual String GetClassSignature() const override;

			virtual void StoreData( Storage< Real >::Frame& frame ) override;

		private:
			struct Limit
			{
				Limit( const PropNode& props, sim::Model& model );
				sim::Dof& dof;
				Range< Degree > range;
				Range< Degree > velocity_range;
				Real squared_range_penalty;
				Real abs_range_penalty;
				Real squared_velocity_range_penalty;
				Real abs_velocity_range_penalty;
				Real squared_force_penalty;
				Statistic<> penalty;
			};

			PropNode m_Report;
			std::vector< Limit > m_Limits;
		};
	}
}
