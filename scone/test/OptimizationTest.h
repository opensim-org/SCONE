#pragma once

#include "..\opt\Optimization.h"
#include "ExampleObjective.h"
#include "..\opt\opt.h"
#include "..\cs\SimulationObjective.h"

using namespace scone;

void OptimizationTest()
{
	// register new objective
	ExampleObjective::RegisterFactory();
	cs::RegisterFactoryTypes();

	opt::OptimizerSP opt = opt::CreateOptimizerFromXml( "config/optimization_test.xml" );

	opt->Run();
}
