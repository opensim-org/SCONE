#pragma once

#include "sim_simbody.h"
#include "scone/model/Simulation.h"

namespace scone
{
	class SCONE_API Simulation_Simbody : public Simulation
	{
	public:
		Simulation_Simbody( const PropNode& props );
		virtual ~Simulation_Simbody() { };

		virtual void AdvanceSimulationTo( double time ) override;
	};
}
