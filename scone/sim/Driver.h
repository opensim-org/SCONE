#pragma once

#include <memory>
#include "sim.h"

namespace scone
{
	namespace sim
	{
		class SIM_API Driver
		{
		public:
			Driver();
			virtual ~Driver();

			virtual std::unique_ptr< class Simulation > CreateWorld() = 0;
		};
	}
}
