#include "stdafx.h"
#include "Factories.h"

namespace scone
{
	static FunctionFactory g_FunctionFactory;
	CS_API FunctionFactory& GetFunctionFactory() { return g_FunctionFactory; }
	CS_API FunctionUP CreateFunction( const PropNode& props, opt::ParamSet& par ) {
		return FunctionUP( GetFunctionFactory().Create( props )( props, par ) );
	}
}
