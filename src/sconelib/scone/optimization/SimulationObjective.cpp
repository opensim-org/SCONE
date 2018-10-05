/*
** SimulationObjective.cpp
**
** Copyright (C) 2013-2018 Thomas Geijtenbeek. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "SimulationObjective.h"

#include "scone/core/Exception.h"
#include "scone/model/Model.h"

#include "scone/core/version.h"
#include "scone/core/string_tools.h"
#include "scone/core/system_tools.h"
#include "scone/core/Factories.h"

namespace scone
{
	SimulationObjective::SimulationObjective( const PropNode& props ) :
	ModelObjective( props )
	{
		auto model = InitializeModelObjective( props );

		INIT_PROP( props, max_duration, 1e12 );

		signature_ += stringf( ".D%.0f", max_duration );
	}

	SimulationObjective::~SimulationObjective()
	{}

	scone::fitness_t SimulationObjective::EvaluateModel( Model& m ) const
	{
		m.SetSimulationEndTime( GetDuration() );
		AdvanceSimulationTo( m, GetDuration() );
		return m.GetMeasure()->GetWeightedResult( m );
	}

	void SimulationObjective::AdvanceSimulationTo( Model& m, TimeInSeconds t ) const
	{
		m.AdvanceSimulationTo( t );
	}
}
