#include "Measure.h"

namespace scone
{
	Measure::Measure( const PropNode& props, Params& par, Model& model, const Location& loc ) : Controller( props, par, model, loc )
	{
		INIT_PROP( props, name, props.get< string >( "type" ) );
		INIT_PROP( props, weight, 1.0 );
		INIT_PROP( props, threshold, 0.0 );
		INIT_PROP( props, offset, 0.0 );
		INIT_PROP( props, minimize, true );
	}

	double Measure::GetWeightedResult( Model& model )
	{
		double result = GetResult( model ) + GetOffset();
		if ( minimize && GetThreshold() != 0 && result < GetThreshold() )
			return 0;
		else return result * GetWeight();
	}

	bool Measure::PerformAnalysis( const Model& model, double timestamp )
	{
		// TODO: cleanup, rename UpdateMeasure into PerformAnalysis
		return UpdateMeasure( model, timestamp );
	}
}
