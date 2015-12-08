#pragma once
#include "../sim/sim.h"
#include "../core/InitFromPropNode.h"
#include "../sim/Controller.h"

namespace scone
{
	namespace cs
	{
		class Measure : public sim::Controller
		{
		public:
			Measure( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area );
			virtual ~Measure() { };

			virtual double GetResult( sim::Model& model ) = 0;
			virtual PropNode GetReport() { return PropNode(); }

			virtual bool IsActive( const sim::Model& model, TimeInSeconds timestamp ) const { return timestamp >= start_time; }

		private:
			TimeInSeconds start_time;
		};
	}
}
