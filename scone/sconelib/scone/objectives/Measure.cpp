#include "Measure.h"

namespace scone
{
	namespace cs
	{
		Measure::Measure( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area ) : Controller( props, par, model, area )
		{
			INIT_PROPERTY( props, start_time, 0.0 );
			INIT_PROPERTY( props, name, String( "" ) );
			INIT_PROPERTY( props, weight, 1.0 );
			INIT_PROPERTY( props, threshold, 0.0 );
			INIT_PROPERTY( props, offset, 0.0 );
		}
	}
}
