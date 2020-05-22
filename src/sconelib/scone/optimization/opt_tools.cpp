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
#include "xo/serialization/char_stream.h"

using xo::timer;

namespace scone
{
	SCONE_API bool LogUnusedProperties( const PropNode& pn )
	{
		// report unused properties
		if ( pn.count_unaccessed() > 0 )
		{
			log::warning( "Warning, unused properties:" );
			xo::log_unaccessed( pn );
			return true;
		}
		else return false;
	}

	PropNode EvaluateScenario( const PropNode& scenario_pn, const path& par_file, const path& output_base )
	{
		bool store_data = !output_base.empty();

		auto opt = CreateOptimizer( scenario_pn, par_file.parent_path() );
		auto mo = dynamic_cast<ModelObjective*>( &opt->GetObjective() );

		// report unused properties
		LogUnusedProperties( scenario_pn );

		// create model
		bool has_par_file = par_file.extension_no_dot() == "par";
		ModelUP model = has_par_file ? mo->CreateModelFromParFile( par_file ) : mo->CreateModelFromParams( mo->info() );

		model->SetStoreData( store_data );

		timer tmr;
		auto result = mo->EvaluateModel( *model, xo::stop_token() );
		auto duration = tmr().seconds();

		// write results
		if ( store_data )
		{
			auto files = model->WriteResults( output_base );
			log::info( "Results written to " + output_base.str() + "*" );
		}

		// collect statistics
		PropNode statistics;
		statistics.set( "result", mo->GetReport( *model ) );
		statistics.set( "simulation time", model->GetTime() );
		statistics.set( "performance (x real-time)", model->GetTime() / duration );

		return statistics;
	}

	void BenchmarkScenario( const PropNode& scenario_pn, const path& file, size_t evals )
	{
		auto opt = CreateOptimizer( scenario_pn, file.parent_path() );
		auto mo = dynamic_cast<ModelObjective*>( &opt->GetObjective() );
		auto par = SearchPoint( mo->info() );

		static auto first_run_time = scone::GetDateTimeAsString();
		auto stats_file = file.parent_path() / "perf" / xo::get_computer_name() / first_run_time + "." + file.stem();
		auto stats_base_file = file.parent_path() / "perf" / xo::get_computer_name() / file.stem();
		for ( index_t idx = 0; idx < evals; ++idx )
		{
			//log::info( file, " --> ", idx );
			auto model = mo->CreateModelFromParams( par );
			model->SetStoreData( false );
			model->AdvanceSimulationTo( model->GetSimulationEndTime() );
			if ( evals > 1 )
				model->UpdatePerformanceStats( stats_file );
		}

		auto res = load_string( stats_file + ".stats" );
		if ( xo::file_exists( stats_base_file + ".stats" ) )
		{
			xo::char_stream rstr( std::move( res ) );
			xo::char_stream bstr( load_string( stats_base_file + ".stats" ) );
			while ( rstr.good() )
			{
				string rname, bname;
				double rbest, rmean, rstd;
				double bbest, bmean, bstd;
				rstr >> rname >> rbest >> rmean >> rstd;
				bstr >> bname >> bbest >> bmean >> bstd;
				if ( rstr.good() )
				{
					auto meanperc = 100 * ( rmean - bmean ) / bmean;
					auto meanstd = ( rmean - bmean ) / rstd;
					log::level l;
					if ( meanstd < -1 ) l = log::level::warning;
					else if ( meanstd > 1 ) l = log::level::error;
					else l = log::level::info;
					log::message( l, xo::stringf( "%-32s\t%5.0fns\t%+5.0fns\t%+6.2f%%\t%+6.2fS\t%6.2f", rname.c_str(),
						rmean, rmean - bmean, meanperc, meanstd, rstd ) );
				}
			}
		}
		else log::info( "Benchmark results:\n" + res );
	}

	path FindScenario( const path& file )
	{
		if ( file.extension_no_dot() == "scone" || file.extension_no_dot() == "xml" )
			return file;
		auto folder = file.parent_path();
		return xo::find_file( { path( file ).replace_extension( "scone" ), folder / "config.scone", folder / "config.xml" } );
	}
}
