#include "Measure.h"

namespace scone
{
	Measure::Measure( const PropNode& props, Params& par, Model& model, const Locality& area ) : Controller( props, par, model, area )
	{
		INIT_PROPERTY( props, name, String( "" ) );
		INIT_PROPERTY( props, weight, 1.0 );
		INIT_PROPERTY( props, threshold, 0.0 );
		INIT_PROPERTY( props, offset, 0.0 );
		INIT_PROPERTY( props, minimize, true );
	}

	bool Measure::PerformAnalysis( const Model& model, double timestamp )
	{
		// TODO: cleanup, rename UpdateMeasure into PerformAnalysis
		return UpdateMeasure( model, timestamp );
	}
}
