#pragma once

#include "sim_simbody.h"
#include "../Simulation.h"
#include "../../core/PropNodeFactory.h"

namespace scone
{
	namespace sim
	{
		class SCONE_SIM_SIMBODY_API Simulation_Simbody : public Simulation, public Factoryable< Simulation, Simulation_Simbody >
		{
		public:
			Simulation_Simbody( const PropNode& props );;
			virtual ~Simulation_Simbody() { };

			virtual void AdvanceSimulationTo( double time ) override;
		};
	}
}
