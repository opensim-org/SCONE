#pragma once

#include "opt.h"
#include "../core/Factory.h"
#include "Optimizer.h"

namespace scone
{
	namespace opt
	{
		DECLARE_FACTORY( Optimizer, ( const PropNode& ) );
		OPT_API OptimizerFactory& GetOptimizerFactory();
		OPT_API OptimizerUP CreateOptimizer( const PropNode& props );

		DECLARE_FACTORY( Objective, ( const PropNode&, ParamSet& ) );
		OPT_API ObjectiveFactory& GetObjectiveFactory();
		OPT_API ObjectiveUP CreateObjective( const PropNode& props, ParamSet& par );
	}
}
