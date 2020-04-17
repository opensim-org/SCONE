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
		constexpr double step_size = 0.5;
		m.SetSimulationEndTime( GetDuration() );
		for ( TimeInSeconds t = step_size; !m.HasSimulationEnded(); t += step_size )
		{
			if ( st.stop_requested() )
				return xo::error_message( "Simulation canceled" );
			AdvanceSimulationTo( m, t );
		}
		return m.GetMeasure()->GetWeightedResult( m );
	}

	void SimulationObjective::AdvanceSimulationTo( Model& m, TimeInSeconds t ) const
	{
		m.AdvanceSimulationTo( t );
	}
}
