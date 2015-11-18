#include "stdafx.h"

#include "Factories.h"

namespace scone
{
	namespace opt
	{
		static OptimizerFactory g_OptimizerFactory;
		OPT_API OptimizerFactory& GetOptimizerFactory() { return g_OptimizerFactory; }
		OPT_API OptimizerUP CreateOptimizer( const PropNode& props ) {
			return OptimizerUP( GetOptimizerFactory().Create( props )( props ) );
		}

		static ObjectiveFactory g_ObjectiveFactory;
		OPT_API ObjectiveFactory& GetObjectiveFactory() { return g_ObjectiveFactory; }
		OPT_API ObjectiveUP CreateObjective( const PropNode& props, ParamSet& par ) {
			return ObjectiveUP( GetObjectiveFactory().Create( props )( props, par ) );
		}
	}
}
