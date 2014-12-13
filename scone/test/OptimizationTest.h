#pragma once

#include "..\opt\Optimization.h"
#include "ExampleObjective.h"
#include "..\opt\opt.h"

using namespace scone;

void OptimizationTest()
{
	ExampleObjective::RegisterFactory();

	PropNode prop;
	prop.FromXmlFile( "config/example_optimization.xml" );

	opt::OptimizerSP opt = opt::CreateOptimizer( prop, "Optimizer" );

	opt->Run();
}
