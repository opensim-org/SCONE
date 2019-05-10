/*
** opt_tools.cpp
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "opt_tools.h"
#include "scone/core/types.h"
#include "scone/core/Factories.h"
#include "scone/optimization/SimulationObjective.h"
#include "scone/core/profiler_config.h"

#include "xo/time/timer.h"
#include "xo/container/prop_node_tools.h"
#include "xo/filesystem/filesystem.h"

using xo::timer;

namespace scone
{
	OptimizerUP PrepareOptimization( const PropNode& scenario_pn, const path& scenario_dir )
	{
		// create optimizer and report unused parameters
		xo::current_path( scenario_dir ); // external resources are copied from current path
		OptimizerUP o = CreateOptimizer( scenario_pn );

		// report unused properties
		if ( scenario_pn.count_unaccessed() > 0 )
		{
			log::warning( "Warning, unused properties:" );
			xo::log_unaccessed( scenario_pn );
		}

		// return created optimizer
		return std::move( o );
	}

	PropNode EvaluateScenario( const PropNode& scenario_pn, const path& par_file, const path& output_base )
	{
		bool store_data = !output_base.empty();
		current_path( par_file.parent_path() );

		auto optProp = FindFactoryProps( GetOptimizerFactory(), scenario_pn, "Optimizer" );
		auto objProp = FindFactoryProps( GetObjectiveFactory(), optProp.props(), "Objective" );
		ObjectiveUP obj = CreateObjective( objProp );
		SimulationObjective& so = dynamic_cast<SimulationObjective&>( *obj );

		// report unused properties
		if ( objProp.props().count_unaccessed() > 0 )
		{
			log::warning( "Warning, unused properties:" );
			xo::log_unaccessed( objProp.props() );
		}

		// create model
		ModelUP model;
		if ( par_file.empty() || par_file.extension() == "scone" )
		{
			// no par file was given, try to use init_file
			// IMPORTANT: this uses the parameter MEAN of the init_file
			// as to be consistent with running a scenario from inside SCONE studio
			// TODO: combine this code with CreateModelObjective, since the same is happening there
			if ( auto init_file = optProp.props().try_get< path >( "init_file" ) )
				so.info().import_mean_std( *init_file, optProp.props().get< bool >( "use_init_file_std", true ) );
			SearchPoint searchPoint( so.info() );
			model = so.CreateModelFromParams( searchPoint );
		}
		else model = so.CreateModelFromParFile( par_file );

		// set data storage
		SCONE_ASSERT( model );
		model->SetStoreData( store_data );

		timer tmr;
		double result = so.EvaluateModel( *model );
		auto duration = tmr().seconds<double>();

		// write results
		if ( store_data )
		{
			auto files = model->WriteResults( output_base );
			log::info( "Results written to " + output_base.str() + "*" );
		}

		// collect statistics
		PropNode statistics;
		statistics.set( "result", so.GetReport( *model ) );
		statistics.set( "simulation time", model->GetTime() );
		statistics.set( "performance (x real-time)", model->GetTime() / duration );

		log::info( statistics );

		return statistics;
	}

	path FindScenario( const path& file )
	{
		if ( file.extension() == "scone" || file.extension() == "xml" )
			return file;

		auto folder = file.parent_path();
		return xo::find_file( { path( file ).replace_extension( "scone" ), folder / "config.scone", folder / "config.xml" } );
	}
}
