#include "Measure.h"

namespace scone
{
	Measure::Measure( const PropNode& props, Params& par, Model& model, const Locality& area ) : Controller( props, par, model, area )
	{
		INIT_PROP( props, name, String( "" ) );
		INIT_PROP( props, weight, 1.0 );
		INIT_PROP( props, threshold, 0.0 );
		INIT_PROP( props, offset, 0.0 );
		INIT_PROP( props, minimize, true );
	}

	bool Measure::PerformAnalysis( const Model& model, double timestamp )
	{
		// TODO: cleanup, rename UpdateMeasure into PerformAnalysis
		return UpdateMeasure( model, timestamp );
	}
}
