/*
** Measure.cpp
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "Measure.h"

namespace scone
{
	Measure::Measure( const PropNode& props, Params& par, Model& model, const Location& loc ) : Controller( props, par, model, loc )
	{
		INIT_PROP( props, name, "" );
		INIT_PROP( props, weight, 1.0 );
		INIT_PROP( props, threshold, 0.0 );
		INIT_PROP( props, offset, 0.0 );
		INIT_PROP( props, minimize, true );
	}

	double Measure::GetResult( Model& model )
	{
		if ( !result )
			result = ComputeResult( model );
		return *result;
	}

	double Measure::GetWeightedResult( Model& model )
	{
		if ( !result )
			result = ComputeResult( model );

		if ( minimize && GetThreshold() != 0 && ( *result + GetOffset() ) < GetThreshold() )
			return 0;
		else return GetWeight() * ( *result + GetOffset() );
	}

	bool Measure::PerformAnalysis( const Model& model, double timestamp )
	{
		// TODO: cleanup, rename UpdateMeasure into PerformAnalysis
		return UpdateMeasure( model, timestamp );
	}
}
