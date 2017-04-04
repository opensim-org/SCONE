#pragma once

#include "Model.h"
#include "scone/core/Factory.h"
#include "Area.h"

namespace scone
{
	namespace sim
	{
		//SCONE_API ModelUP CreateModel( const PropNode&, opt::ParamSet& );
		SCONE_API SensorUP CreateSensor( const PropNode&, opt::ParamSet&, Model&, const Area& );
		DECLARE_FACTORY( SCONE_API, Model, ( const PropNode&, opt::ParamSet& ) );
		//DECLARE_FACTORY( SCONE_API, Sensor, ( const PropNode&, opt::ParamSet&, Model&, const Area& ) );
	}
}
