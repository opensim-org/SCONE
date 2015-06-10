#include "stdafx.h"
#include "Factories.h"

namespace scone
{
	static FunctionFactory g_FunctionFactory;
	CS_API FunctionFactory& GetFunctionFactory() { return g_FunctionFactory; }
	CS_API FunctionUP CreateFunction( const PropNode& props, opt::ParamSet& par ) {
		return FunctionUP( GetFunctionFactory().Create( props )( props, par ) );
	}

	namespace cs
	{
		static ReflexFactory g_ReflexFactory;
		CS_API ReflexFactory& GetReflexFactory() { return g_ReflexFactory; }
		CS_API ReflexUP CreateReflex( const PropNode& props, opt::ParamSet& par, sim::Model& model, sim::Actuator& target, sim::ChannelSensor& source ) {
			return ReflexUP( GetReflexFactory().Create( props )( props, par, model, target, source ) );
		}
	}
}
