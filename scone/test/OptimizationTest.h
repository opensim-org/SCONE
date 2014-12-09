#pragma once

#include "..\cs\Optimization.h"

using namespace scone;

void OptimizationTest()
{
	cs::Optimization opt;
	opt.Run("config/optimization_test.xml");
};
