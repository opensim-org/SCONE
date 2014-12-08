#pragma once
#include "Measure.h"

namespace scone
{
	namespace cs
	{
		class JumpingMeasure : public Measure
		{
		public:
			JumpingMeasure() { };
			virtual ~JumpingMeasure() { };

			virtual bool Update( sim::ModelSP model, double timestamp ) override;

			virtual double GetValue() override;

		};
	}
}
