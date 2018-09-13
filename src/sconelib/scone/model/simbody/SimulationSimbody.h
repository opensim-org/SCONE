#pragma once

#include "sim_simbody.h"
#include "scone/model/Simulation.h"

namespace scone
{
	class SCONE_API SimulationSimbody : public Simulation
	{
	public:
		SimulationSimbody( const PropNode& props );
		virtual ~SimulationSimbody() { };

		virtual void AdvanceSimulationTo( double time ) override;
	};
}
