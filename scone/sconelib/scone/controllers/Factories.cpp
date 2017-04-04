#include "Factories.h"
#include "flut/factory.hpp"
#include "scone/controllers/GaitStateController.h"
#include "scone/controllers/TimeStateController.h"
#include "scone/controllers/PerturbationController.h"
#include "scone/controllers/FeedForwardController.h"
#include "scone/controllers/ReflexController.h"
#include "scone/controllers/MetaReflexController.h"

namespace scone
{
	static FunctionFactory g_FunctionFactory;
	SCONE_API FunctionFactory& GetFunctionFactory() { return g_FunctionFactory; }
	SCONE_API FunctionUP CreateFunction( const PropNode& props, opt::ParamSet& par ) {
		return FunctionUP( GetFunctionFactory().Create( props )( props, par ) );
	}

	namespace cs
	{
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
				g_ControllerFactory.register_class< FeedForwardController >();
				g_ControllerFactory.register_class< GaitStateController >();
				g_ControllerFactory.register_class< ReflexController >();
				g_ControllerFactory.register_class< TimeStateController >();
				g_ControllerFactory.register_class< MetaReflexController >();
				g_ControllerFactory.register_class< PerturbationController >();
			}

			return g_ControllerFactory[ props.get< String >( "type" ) ]( props, par, model, target_area );
		}
	}
}
