#pragma once

#include "../core/Factory.h"
#include "../core/PropNode.h"
#include "../opt/ParamSet.h"

#include "Function.h"
#include "Reflex.h"

namespace scone
{
	DECLARE_FACTORY( CS_API, Function, ( const PropNode&, opt::ParamSet& ) );

	namespace cs
	{
		DECLARE_FACTORY( CS_API, Reflex, ( const PropNode&, opt::ParamSet&, sim::Model&, sim::Actuator&, sim::Sensor& ) );
	}
}
