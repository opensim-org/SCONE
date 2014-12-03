#pragma once

#include "sim.h"
#include "Model.h"
#include <memory>

namespace scone
{
	namespace sim
	{
		class SIM_API Simulation
		{
		public:
			Simulation();
			virtual ~Simulation();

			virtual ModelSP CreateModel( const String& filename ) = 0;
		};
	}
}
