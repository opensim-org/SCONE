#pragma once

#include "../core/Factory.h"
#include "../core/PropNode.h"
#include "../opt/ParamSet.h"

#include "Function.h"

namespace scone
{
	DECLARE_FACTORY( Function, ( const PropNode&, opt::ParamSet& ) );
	CS_API FunctionFactory& GetFunctionFactory();
	CS_API FunctionUP CreateFunction( const PropNode& props, opt::ParamSet& );
}
