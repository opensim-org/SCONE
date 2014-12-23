#pragma once

#include "../sim/simbody/Simulation_Simbody.h"
#include "../sim/sim.h"
#include "../sim/Model.h"
#include "../core/Log.h"
#include "../core/Timer.h"
#include "../core/Propertyable.h"

using namespace scone;

void SimulationTest()
{
	cs::RegisterFactoryTypes();

	PropNode props = LoadXmlFile( "config/simulation_test.xml" );
	sim::SimulationUP s = CreateFromPropNode< sim::Simulation >( props );

	Timer t;
	SCONE_LOG( "Starting simulation" );
	s->AdvanceSimulationTo( 0.5 );
	SCONE_LOG( "Simulation ended in " << t.GetTime() );
}
