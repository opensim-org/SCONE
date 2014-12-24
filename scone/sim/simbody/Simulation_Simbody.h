#pragma once

#include "sim_simbody.h"
#include "..\Simulation.h"

namespace scone
{
	namespace sim
	{
		class SCONE_SIM_SIMBODY_API Simulation_Simbody : public Simulation, public Factoryable< Simulation, Simulation_Simbody >
		{
		public:
			Simulation_Simbody() { };
			virtual ~Simulation_Simbody() { };

			virtual void ProcessProperties( const PropNode& props );
			virtual void AdvanceSimulationTo( double time ) override;
		};
	}
}
