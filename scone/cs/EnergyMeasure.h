#pragma once
#include "Measure.h"

namespace scone
{
	namespace cs
	{
		class EnergyMeasure : public Measure
		{
		public:
			EnergyMeasure( const PropNode& props ) : Measure( props ) { };
			virtual ~EnergyMeasure() { };

			virtual void UpdateControls( sim::Model& model, double timestamp ) override;
			virtual double GetResult(sim::Model& model ) override;

		protected:
		private:
		};
	}
}
