#pragma once
#include "../sim/sim.h"
#include "../core/Propertyable.h"
#include "../sim/Controller.h"

namespace scone
{
	namespace cs
	{
		class Measure : public sim::Controller
		{
		public:
			Measure( const PropNode& props );
			virtual ~Measure() { };

			virtual void UpdateControls( sim::Model& model, double timestamp ) = 0;
			virtual double GetResult( sim::Model& model ) = 0;

		private:
		};
	}
}
