#include "stdafx.h"
#include "Measure.h"

namespace scone
{
	namespace cs
	{
		Measure::Measure( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area ) : Controller( props, par, model, area )
		{
			INIT_PROPERTY( props, start_time, 0.0 );
		}
	}
}
