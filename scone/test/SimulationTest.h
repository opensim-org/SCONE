#pragma once

#include "../sim/simbody/Simulation_Simbody.h"
#include "../sim/sim.h"
#include "../sim/Model.h"
#include "../core/Log.h"
#include "../core/Timer.h"

using namespace scone;

void SimulationTest()
{
	sim::Simulation_Simbody s;
	sim::Model& m = s.AddModel( "models/jumper10dof24musc.osim");

	SCONE_LOG( "Starting simulation" );
	Timer t;
	s.AdvanceSimulationTo( 0.5 );
	SCONE_LOG( "Simulation ended in " << t.GetTime() );
}
