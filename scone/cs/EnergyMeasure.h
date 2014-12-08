#pragma once
#include "Measure.h"

namespace scone
{
	namespace cs
	{
		class EnergyMeasure : public Measure
		{
		public:
			EnergyMeasure() { };
			virtual ~EnergyMeasure() { };

			virtual bool Update( sim::ModelSP model, double timestamp ) override;

			virtual void ProcessPropNode( PropNode& props ) override;

			virtual double GetValue() override;


		protected:
		private:
		};
	}
}
