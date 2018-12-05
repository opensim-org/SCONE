/*
** opt_tools.cpp
**
** Copyright (C) 2013-2018 Thomas Geijtenbeek. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "opt_tools.h"
#include "scone/core/types.h"
#include "scone/core/Factories.h"
#include "scone/optimization/SimulationObjective.h"
#include "scone/core/Profiler.h"

#include "xo/time/timer.h"
#include "xo/container/prop_node_tools.h"
#include "xo/filesystem/filesystem.h"

using xo::timer;

namespace scone
{
	SCONE_API OptimizerUP PrepareOptimization( const PropNode& scenario_pn, const path& scenario_file )
	{
		// create optimizer and report unused parameters
		xo::current_path( scenario_file.parent_path() ); // external resources are copied from current path
		OptimizerUP o = CreateOptimizer( scenario_pn.get_child( "Optimizer" ) );

		// report unused properties
		if ( scenario_pn.count_unaccessed() > 0 )
		{
			log::warning( "Warning, unused properties:" );
			xo::log_unaccessed( scenario_pn );
		}

		// return created optimizer
		return std::move( o );
	}

	PropNode SCONE_API EvaluateScenario( const PropNode& scenario_pn, const path& par_file, const path& output_base )
	{
		current_path( par_file.parent_path() );

		const PropNode& optProp = scenario_pn[ "Optimizer" ];
		const PropNode& objProp = optProp[ "Objective" ];
		ObjectiveUP obj = CreateObjective( objProp );
		SimulationObjective& so = dynamic_cast<SimulationObjective&>( *obj );

		// report unused properties
		if ( objProp.count_unaccessed() > 0 )
		{
			log::warning( "Warning, unused properties:" );
			xo::log_unaccessed( objProp );
		}

		// create model
		ModelUP model;
		if ( par_file.empty() || par_file.extension() == "scone" )
		{
			// no par file was given, try to use init_file
			if ( auto init_file = optProp.try_get< path >( "init_file" ) )
				model = so.CreateModelFromParFile( *init_file );
			else model = so.CreateModelFromParams( SearchPoint( so.info() ) );
		}
		else model = so.CreateModelFromParFile( par_file );

		// set data storage
		SCONE_ASSERT( model );
		model->SetStoreData( true );
		Profiler::GetGlobalInstance().Reset();

		timer tmr;
		double result = so.EvaluateModel( *model );
		auto duration = tmr.seconds();

		// collect statistics
		PropNode statistics;
		statistics.set( "result", so.GetReport( *model ) );
		statistics.set( "simulation time", model->GetTime() );
		statistics.set( "performance (x real-time)", model->GetTime() / duration );

		log::info( statistics );

		// write results
		auto files = model->WriteResults( output_base );
		log::info( "Results written to " + output_base.string() + "*" );

		return statistics;
	}

	SCONE_API path FindScenario( const path& file )
	{
		if ( file.extension() == "scone" || file.extension() == "xml" )
			return file;

		auto folder = file.parent_path();
		return xo::find_file( { path( file ).replace_extension( "scone" ), folder / "config.scone", folder / "config.xml" } );
	}
}
