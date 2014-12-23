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

			virtual bool UpdateControls( sim::Model& model, double timestamp ) override;

			virtual double GetValue() override;

		};
	}
}
