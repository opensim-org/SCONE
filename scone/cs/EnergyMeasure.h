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

			virtual bool UpdateControls( sim::Model& model, double timestamp ) override;

			virtual void ProcessProperties( const PropNode& props ) override;

			virtual double GetResult(sim::Model& model ) override;


		protected:
		private:
		};
	}
}
