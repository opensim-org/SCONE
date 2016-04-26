#include "Factories.h"

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
	}
}
