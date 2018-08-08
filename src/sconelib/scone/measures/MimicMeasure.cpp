#include "MimicMeasure.h"

namespace scone
{
	
	MimicMeasure::MimicMeasure( const PropNode& props, Params& par, Model& model, const Locality& area ) :
	Measure( props, par, model, area )
	{
	}

	bool MimicMeasure::UpdateMeasure( const Model& model, double timestamp )
	{
		return false;
	}

	double MimicMeasure::GetResult( Model& model )
	{
		return 0.0;
	}

	String MimicMeasure::GetClassSignature() const
	{
		return String( "M" );
	}
}
