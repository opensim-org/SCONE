#pragma once

#include "cs_fwd.h"

#include "scone/core/Factory.h"
#include "scone/core/PropNode.h"
#include "scone/opt/ParamSet.h"

#include "Function.h"
#include "Reflex.h"

namespace scone
{
	DECLARE_FACTORY( SCONE_API, Function, ( const PropNode&, opt::ParamSet& ) );

	namespace cs
	{
		DECLARE_FACTORY( SCONE_API, Reflex, ( const PropNode&, opt::ParamSet&, sim::Model&, const sim::Area& ) );
	}
}
