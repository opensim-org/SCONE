/*
** Measure.cpp
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "Measure.h"
#include "xo/numerical/constants.h"

namespace scone
{
	Measure::Measure( const PropNode& props, Params& par, const Model& model, const Location& loc ) :
		Controller( props, par, const_cast<Model&>( model ), loc ) // model is no longer const in Controller parent class
	{
		INIT_PROP( props, name, "" );
		INIT_PROP( props, weight, 1.0 );
		INIT_PROP( props, threshold, 0.0 );
		INIT_PROP( props, result_offset, 0.0 );
		INIT_PROP( props, minimize, true );
	}

	double Measure::GetResult( const Model& model )
	{
		if ( !result )
			result = ComputeResult( model );
		return *result;
	}

	double Measure::GetWeightedResult( const Model& model )
	{
		if ( !result )
			result = ComputeResult( model );

		if ( minimize && GetThreshold() != 0 && ( *result + GetOffset() ) < GetThreshold() )
			return 0;
		else return GetWeight() * ( *result + GetOffset() );
	}

	const String& Measure::GetName() const
	{
		if ( name.empty() )
			name = xo::get_clean_type_name( *this );
		return name;
	}

	bool Measure::PerformAnalysis( const Model& model, double timestamp )
	{
		// #todo: cleanup, rename UpdateMeasure into PerformAnalysis
		return UpdateMeasure( model, timestamp );
	}

	double Measure::WorstResult() const
	{
		return minimize ? xo::constants<double>::max() : xo::constants<double>::lowest();
	}

}
