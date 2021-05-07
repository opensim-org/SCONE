/*
** BalanceMeasure.cpp
**
** Copyright (C) 2013-2021 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "BalanceMeasure.h"
#include "scone/model/Model.h"

namespace scone
{
	
	BalanceMeasure::BalanceMeasure( const PropNode& props, Params& par, const Model& model, const Location& loc ) :
	Measure( props, par, model, loc )
	{
		INIT_PROP( props, termination_height, 0.5 );

		m_InitialHeight = model.GetComHeight();
	}

	bool BalanceMeasure::UpdateMeasure( const Model& model, double timestamp )
	{
		double pos = model.GetComHeight();
		if ( pos < termination_height * m_InitialHeight )
			return true;

		return false;
	}

	double BalanceMeasure::ComputeResult( const Model& model )
	{
		return std::max( 0.0, ( model.GetSimulationEndTime() - model.GetTime() ) / model.GetSimulationEndTime() );
	}

	String BalanceMeasure::GetClassSignature() const
	{
		return "B";
	}
}
