#pragma once

#include "Measure.h"
#include "../core/InitFromPropNode.h"
#include "../core/Range.h"
#include "../sim/Dof.h"

namespace scone
{
	namespace cs
	{
		class DofLimitMeasure : public Measure
		{
		public:
			DofLimitMeasure( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area );
			virtual ~DofLimitMeasure();

			virtual void UpdateControls( sim::Model& model, double timestamp ) override;
			virtual double GetResult( sim::Model& model ) override;

		private:
			virtual String GetSignature() override;

			struct Limit
			{
				Limit( const PropNode& props, sim::Model& model );

				sim::Dof& dof;
				Range< Real > range;
				Real penalty;
			};

			std::vector< Limit > m_Limits;
		};
	}
}
