#pragma once

#include "Model.h"
#include "../core/Factory.h"
#include "Area.h"

namespace scone
{
	namespace sim
	{
		DECLARE_FACTORY( SCONE_SIM_API, Model, ( const PropNode&, opt::ParamSet& ) );
		DECLARE_FACTORY( SCONE_SIM_API, Controller, ( const PropNode&, opt::ParamSet&, Model&, const Area& ) );
	}
}
