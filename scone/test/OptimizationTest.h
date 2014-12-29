#pragma once

#include "ExampleObjective.h"
#include "..\opt\Optimizer.h"
#include "..\cs\SimulationObjective.h"

using namespace scone;

void OptimizationTest()
{
	// register new objective
	ExampleObjective::RegisterFactory();
	cs::RegisterFactoryTypes();

	//opt::OptimizerUP opt = opt::CreateOptimizerFromXml( "config/example_optimization.xml" );
	opt::OptimizerUP opt = opt::CreateOptimizerFromXml( "config/optimization_test.xml" );

	opt->Run();
}
