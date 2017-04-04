#pragma once

#include "scone/core/Factory.h"
#include "Optimizer.h"
#include "flut/factory.hpp"

namespace scone
{
	namespace opt
	{
		SCONE_API OptimizerUP CreateOptimizer( const PropNode& prop );
		SCONE_API ObjectiveUP CreateObjective( const PropNode& prop, ParamSet& par );

		SCONE_API flut::factory< Objective, const PropNode&, ParamSet& >& GetObjectiveFactory();
	}
}
