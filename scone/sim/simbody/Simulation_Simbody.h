#pragma once

#include "simbody.h"
#include "..\Simulation.h"

#include <memory>

namespace scone
{
	namespace sim
	{
		class SIM_SIMBODY_API Simulation_Simbody : public Simulation
		{
		public:
			Simulation_Simbody() { };
			virtual ~Simulation_Simbody() { };

			virtual ModelSP CreateModel( const String& filename ) override;
		};
	}
}
