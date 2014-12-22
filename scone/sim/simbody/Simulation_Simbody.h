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
			Simulation_Simbody() : integration_accuracy( 0.00001 ) { };
			virtual ~Simulation_Simbody() { };

			virtual void ProcessProperties(const PropNode& props);
			virtual void AdvanceSimulationTo( double time ) override;

			double integration_accuracy;

		protected:
			virtual ModelUP CreateModel( const String& filename ) override;
		};
	}
}
