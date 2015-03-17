#pragma once

#include "Model.h"
#include "../core/Factory.h"
#include "Area.h"

namespace scone
{
	namespace sim
	{
		DECLARE_FACTORY( Model, ( const PropNode&, opt::ParamSet& ) );
		SCONE_SIM_API ModelFactory& GetModelFactory();
		SCONE_SIM_API ModelUP CreateModel( const PropNode& props, opt::ParamSet& );

		DECLARE_FACTORY( Controller, ( const PropNode&, opt::ParamSet&, Model&, const Area& ) );
		SCONE_SIM_API ControllerFactory& GetControllerFactory();
		SCONE_SIM_API ControllerUP CreateController( const PropNode&, opt::ParamSet&, Model&, const Area& = sim::Area::WHOLE_BODY );
	}
}
