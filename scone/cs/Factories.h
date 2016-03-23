#pragma once

#include "scone/core/Factory.h"
#include "scone/core/PropNode.h"
#include "scone/opt/ParamSet.h"

#include "Function.h"
#include "Reflex.h"

namespace scone
{
	DECLARE_FACTORY( CS_API, Function, ( const PropNode&, opt::ParamSet& ) );

	namespace cs
	{
		DECLARE_FACTORY( CS_API, Reflex, ( const PropNode&, opt::ParamSet&, sim::Model&, const sim::Area& ) );
	}
}
