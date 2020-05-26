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
#include "xo/utility/irange.h"
#include "xo/container/container_algorithms.h"

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

		// run simulations
		xo::flat_map<string, std::vector<TimeInSeconds>> bm_samples;
		for ( index_t idx = 0; idx < evals; ++idx )
		{
			xo::timer t;
			auto model = mo->CreateModelFromParams( par );
			model->SetStoreData( false );
			auto create_model_time = t();
			model->AdvanceSimulationTo( model->GetSimulationEndTime() );
			auto total_time = t();
			auto timings = model->GetBenchmarks();
			for ( const auto& t : timings )
				bm_samples[ t.first ].push_back( t.second.first.seconds() / t.second.second );
			bm_samples[ "TotalTime" ].push_back( total_time.seconds() );
			bm_samples[ "TotalSimTime" ].push_back( ( total_time - create_model_time ).seconds() );
			bm_samples[ "ModelSimTime" ].push_back( timings.front().second.first.seconds() );
			xo::sleep( 100 );
		}

		// read baseline
		auto baseline_file = file.parent_path() / "perf" / xo::get_computer_name() / file.stem() + ".stats";
		bool has_baseline = xo::file_exists( baseline_file );
		xo::flat_map<string, TimeInSeconds> baseline_medians;
		if ( has_baseline )
		{
			xo::char_stream bstr( load_string( baseline_file ) );
			while ( bstr.good() )
			{
				string bname;
				double bmedian, bmean, bstd;
				bstr >> bname >> bmedian >> bmean >> bstd;
				if ( bstr.good() )
					baseline_medians[ bname ] = bmedian;
			}
		}

		// report
		for ( const auto& bms : bm_samples )
		{
			auto rmedian = xo::median( bms.second );
			auto rmeanstd = xo::mean_std( bms.second );
			auto bmedian = baseline_medians[ bms.first ];
			auto medianperc = 100 * ( ( rmedian - bmedian ) / bmedian );
			auto medianstd = ( rmedian - bmedian ) / rmeanstd.second;

			log::info( xo::stringf( "%-32s\t%5.0fns\t%+5.0fns\t%+6.2f%%\t%+6.2fS\t%6.2f", bms.first.c_str(),
				rmedian, rmedian - bmedian, medianperc, medianstd, rmeanstd.second ) );

			if ( !has_baseline )
			{
				auto ostr = std::ofstream( baseline_file.str(), std::ios_base::app );
				ostr << xo::stringf( "%-32s\t%8.2f\t%8.2f\t%8.2f\n", bms.first.c_str(), rmedian, rmeanstd.first, rmeanstd.second );
			}
		}
	}

	path FindScenario( const path& file )
	{
		if ( file.extension_no_dot() == "scone" || file.extension_no_dot() == "xml" )
			return file;
		auto folder = file.parent_path();
		return xo::find_file( { path( file ).replace_extension( "scone" ), folder / "config.scone", folder / "config.xml" } );
	}
}
