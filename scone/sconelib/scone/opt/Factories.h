#pragma once

#include "opt.h"
#include "scone/core/Factory.h"
#include "Optimizer.h"

namespace scone
{
	namespace opt
	{
		DECLARE_FACTORY( SCONE_API, Optimizer, ( const PropNode& ) );
		DECLARE_FACTORY( SCONE_API, Objective, ( const PropNode&, ParamSet& ) );
	}
}
