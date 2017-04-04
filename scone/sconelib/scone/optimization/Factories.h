#pragma once

#include "scone/core/Factory.h"
#include "Optimizer.h"

namespace scone
{
	namespace opt
	{
		//SCONE_AP OptimizerUP CreateOptimizer( const PropNode& );
		DECLARE_FACTORY( SCONE_API, Optimizer, ( const PropNode& ) );
		DECLARE_FACTORY( SCONE_API, Objective, ( const PropNode&, ParamSet& ) );
	}
}
