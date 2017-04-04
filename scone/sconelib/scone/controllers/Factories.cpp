#include "Factories.h"
#include "flut/factory.hpp"
#include "scone/controllers/GaitStateController.h"
#include "scone/controllers/TimeStateController.h"
#include "scone/controllers/PerturbationController.h"
#include "scone/controllers/FeedForwardController.h"
#include "scone/controllers/ReflexController.h"
#include "scone/controllers/MetaReflexController.h"
#include "scone/objectives/GaitMeasure.h"
#include "scone/objectives/GaitCycleMeasure.h"
#include "scone/objectives/CompositeMeasure.h"
#include "scone/objectives/JumpMeasure.h"
#include "scone/objectives/JointLoadMeasure.h"
#include "scone/objectives/ReactionForceMeasure.h"
#include "scone/objectives/PointMeasure.h"
#include "scone/objectives/HeightMeasure.h"

namespace scone
{
	static FunctionFactory g_FunctionFactory;
	SCONE_API FunctionFactory& GetFunctionFactory() { return g_FunctionFactory; }
	SCONE_API FunctionUP CreateFunction( const PropNode& props, opt::ParamSet& par ) {
		return FunctionUP( GetFunctionFactory().Create( props )( props, par ) );
	}

	static ReflexFactory g_ReflexFactory;
	SCONE_API ReflexFactory& GetReflexFactory() { return g_ReflexFactory; }
	SCONE_API ReflexUP CreateReflex( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area ) {
		return ReflexUP( GetReflexFactory().Create( props )( props, par, model, area ) );
	}

	SCONE_API sim::ControllerUP CreateController( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& target_area )
	{
		static flut::factory< sim::Controller, const PropNode&, opt::ParamSet&, sim::Model&, const sim::Area& > g_ControllerFactory;
		if ( g_ControllerFactory.empty() )
		{
			// register controllers
			g_ControllerFactory.register_class< FeedForwardController >();
			g_ControllerFactory.register_class< GaitStateController >();
			g_ControllerFactory.register_class< ReflexController >();
			g_ControllerFactory.register_class< TimeStateController >();
			g_ControllerFactory.register_class< MetaReflexController >();
			g_ControllerFactory.register_class< PerturbationController >();

			// register measures
			g_ControllerFactory.register_class< HeightMeasure >();
			g_ControllerFactory.register_class< GaitMeasure >();
			g_ControllerFactory.register_class< GaitCycleMeasure >();
			g_ControllerFactory.register_class< EffortMeasure >();
			g_ControllerFactory.register_class< DofLimitMeasure >();
			g_ControllerFactory.register_class< CompositeMeasure >();
			g_ControllerFactory.register_class< JumpMeasure >();
			g_ControllerFactory.register_class< JointLoadMeasure >();
			g_ControllerFactory.register_class< ReactionForceMeasure >();
			g_ControllerFactory.register_class< PointMeasure >();
		}

		return g_ControllerFactory[ props.get< String >( "type" ) ]( props, par, model, target_area );
	}
}
