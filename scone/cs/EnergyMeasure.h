#pragma once
#include "Measure.h"

namespace scone
{
	namespace cs
	{
		class EnergyMeasure : public Measure
		{
		public:
			EnergyMeasure( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area ) : Measure( props, par, model, area ) { };
			virtual ~EnergyMeasure() { };

			virtual void UpdateControls( sim::Model& model, double timestamp ) override;
			virtual double GetResult( sim::Model& model ) override;

		protected:
		private:
		};
	}
}
