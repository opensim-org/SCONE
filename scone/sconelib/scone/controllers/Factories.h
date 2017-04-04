#pragma once

#include "scone/core/Factory.h"
#include "scone/core/PropNode.h"
#include "scone/core/Function.h"
#include "scone/controllers/cs_fwd.h"
#include "scone/optimization/ParamSet.h"
#include "scone/model/sim_fwd.h"

#include "Reflex.h"
#include "scone/model/Model.h"
#include "scone/model/Area.h"

namespace scone
{
	SCONE_API sim::ControllerUP CreateController( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& target_area );

	DECLARE_FACTORY( SCONE_API, Function, ( const PropNode&, opt::ParamSet& ) );
	DECLARE_FACTORY( SCONE_API, Reflex, ( const PropNode&, opt::ParamSet&, sim::Model&, const sim::Area& ) );
}
