#pragma once

#include "../sim/simbody/Simulation_Simbody.h"
#include "../sim/sim.h"
#include "../sim/Model.h"

using namespace scone;

void SimulationTest()
{
	sim::Simulation_Simbody s;
	sim::Model& m = s.AddModel( "models/jumper10dof24musc.osim");
}
