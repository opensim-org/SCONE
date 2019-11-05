/*
** SimulationObjective.cpp
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
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
		// #issue84: this model should be kept, but not used by Optimizer
		InitializeModelObjective( props );

		// simulation objectives must have a measure
		SCONE_THROW_IF( !model_->GetMeasure(), "No Measure defined in ModelObjective" );

		INIT_PROP( props, max_duration, 1e12 );

		signature_ += stringf( ".D%.0f", max_duration );
	}

	SimulationObjective::~SimulationObjective()
	{}

	fitness_t SimulationObjective::EvaluateModel( Model& m ) const
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
