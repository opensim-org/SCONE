#pragma once
#include "Measure.h"

namespace scone
{
	namespace cs
	{
		class EnergyMeasure : public Measure
		{
		public:
			EnergyMeasure( const PropNode& props, opt::ParamSet& par, sim::Model& model ) : Measure( props, par, model ) { };
			virtual ~EnergyMeasure() { };

			virtual void UpdateControls( sim::Model& model, double timestamp ) override;
			virtual double GetResult(sim::Model& model ) override;

		protected:
		private:
		};
	}
}
