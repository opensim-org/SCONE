#pragma once

#include "ExampleObjective.h"
#include "../opt/Optimizer.h"
#include "../cs/SimulationObjective.h"

using namespace scone;

void OptimizationTest()
{
	// register new objective
	ExampleObjective::RegisterFactory();
	cs::PerformOptimization( "config/optimization_test.xml" );
}
