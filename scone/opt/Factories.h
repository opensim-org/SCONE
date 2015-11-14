#pragma once

#include "opt.h"
#include "../core/Factory.h"
#include "Optimizer.h"

namespace scone
{
	namespace opt
	{
		DECLARE_FACTORY( OPT_API, Optimizer, ( const PropNode& ) );
		DECLARE_FACTORY( OPT_API, Objective, ( const PropNode&, const ParamSet& ) );
	}
}
