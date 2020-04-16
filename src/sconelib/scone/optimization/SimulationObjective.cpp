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
#ifdef _DEBUG
	constexpr double SIMULATION_STEP = 0.01;
#else
	constexpr double SIMULATION_STEP = 0.25;
#endif

	SimulationObjective::SimulationObjective( const PropNode& pn, const path& find_file_folder ) :
	ModelObjective( pn, find_file_folder )
	{
		// simulation objectives must have a measure
		SCONE_THROW_IF( !model_->GetMeasure(), "No Measure defined in ModelObjective" );

		INIT_PROP( pn, max_duration, 1e12 );

		signature_ += stringf( ".D%.0f", max_duration );
	}

	SimulationObjective::~SimulationObjective()
	{}

	result<fitness_t> SimulationObjective::EvaluateModel( Model& m, const xo::stop_token& st ) const
	{
		m.SetSimulationEndTime( GetDuration() );
		for ( TimeInSeconds t = SIMULATION_STEP; !m.HasSimulationEnded(); t += SIMULATION_STEP )
		{
			if ( st.stop_requested() )
				return xo::error_message( "Optimization Canceled" );
			AdvanceSimulationTo( m, t );
		}
		return m.GetMeasure()->GetWeightedResult( m );
	}

	void SimulationObjective::AdvanceSimulationTo( Model& m, TimeInSeconds t ) const
	{
		m.AdvanceSimulationTo( t );
	}
}
