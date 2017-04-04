#pragma once

#include "Model.h"
#include "scone/core/Factory.h"
#include "Area.h"
#include "flut/factory.hpp"

namespace scone
{
	namespace sim
	{
		SCONE_API SensorUP CreateSensor( const PropNode&, opt::ParamSet&, Model&, const Area& );
		SCONE_API ModelUP CreateModel( const PropNode& prop, opt::ParamSet& par );
	}
}
