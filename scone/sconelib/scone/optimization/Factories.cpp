#include "Factories.h"

#include "flut/factory.hpp"
#include "scone/optimization/CmaOptimizerCCMAES.h"
#include "scone/optimization/CmaOptimizerShark3.h"
#include "scone/optimization/CmaOptimizerCCMAES.h"
#include "scone/objectives/SimulationObjective.h"

namespace scone
{
	namespace opt
	{
		//static OptimizerFactory g_OptimizerFactory;
		//SCONE_API OptimizerFactory& GetOptimizerFactory() { return g_OptimizerFactory; }
		//SCONE_API OptimizerUP CreateOptimizer( const PropNode& props ) {
		//	return OptimizerUP( GetOptimizerFactory().Create( props )( props ) );
		//}

		//static ObjectiveFactory g_ObjectiveFactory;
		//SCONE_API ObjectiveFactory& GetObjectiveFactory() { return g_ObjectiveFactory; }
		//SCONE_API ObjectiveUP CreateObjective( const PropNode& props, ParamSet& par ) {
		//	return ObjectiveUP( GetObjectiveFactory().Create( props )( props, par ) );
		//}

		static flut::factory< Optimizer, const PropNode& > g_OptimizerFactory;
		SCONE_API OptimizerUP CreateOptimizer( const PropNode& prop )
		{
			if ( g_OptimizerFactory.empty() )
			{
				g_OptimizerFactory.register_class< CmaOptimizerCCMAES >( "CmaOptimizer" );
				g_OptimizerFactory.register_class< CmaOptimizerShark3 >();
				g_OptimizerFactory.register_class< CmaOptimizerCCMAES >();
			}
			return g_OptimizerFactory( prop.get< String >( "type" ), prop );
		}

		static flut::factory< Objective, const PropNode&, ParamSet& > g_ObjectiveFactory;
		SCONE_API flut::factory< Objective, const PropNode&, ParamSet& >& GetObjectiveFactory()
		{
			if ( g_ObjectiveFactory.empty() )
			{
				g_ObjectiveFactory.register_class< SimulationObjective >();
			}
			return g_ObjectiveFactory;
		}

		SCONE_API ObjectiveUP CreateObjective( const PropNode& prop, ParamSet& par )
		{
			return GetObjectiveFactory()( prop.get< String >( "type" ), prop, par );
		}
	}
}
