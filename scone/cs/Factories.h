#pragma once

#include "../core/Factory.h"
#include "../core/PropNode.h"
#include "../opt/ParamSet.h"

#include "Function.h"
#include "Reflex.h"

namespace scone
{
	DECLARE_FACTORY( CS_API, Function, ( const PropNode&, opt::ParamSet& ) );
	//CS_API FunctionFactory& GetFunctionFactory();
	//CS_API FunctionUP CreateFunction( const PropNode& props, opt::ParamSet& );

	namespace cs
	{
		DECLARE_FACTORY( CS_API, Reflex, ( const PropNode&, opt::ParamSet&, sim::Model& ) );
		//CS_API ReflexFactory& GetReflexFactory();
		//CS_API FunctionUP CreateReflex( const PropNode& props, opt::ParamSet&, sim::Model& );
	}
}
