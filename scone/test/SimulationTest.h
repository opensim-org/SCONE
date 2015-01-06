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
	sim::ModelUP m = CreateFromPropNode< sim::Model >( props.GetChild( "Model" ) );

	opt::ParamSet par;
	m->ProcessParameters( par );

	Timer t;
	SCONE_LOG( "Starting simulation" );
	m->AdvanceSimulationTo( 1.0 );
	SCONE_LOG( "Simulation ended in " << t.GetTime() );
}
