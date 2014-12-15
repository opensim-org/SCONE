#pragma once

#include "..\opt\Optimization.h"
#include "ExampleObjective.h"
#include "..\opt\opt.h"

using namespace scone;

void OptimizationTest()
{
	// register new objective
	ExampleObjective::RegisterFactory();
	opt::OptimizerSP opt = opt::CreateOptimizerFromXml( "config/example_optimization.xml" );

	opt->Run();
}
