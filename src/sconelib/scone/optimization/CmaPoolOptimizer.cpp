/*
** CmaPoolOptimizer.cpp
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "CmaPoolOptimizer.h"
#include "CmaOptimizerSpot.h"
#include "spot/file_reporter.h"

namespace scone
{
	CmaPoolOptimizer::CmaPoolOptimizer( const PropNode& pn, const PropNode& scenario_pn, const path& scenario_dir ) :
	Optimizer( pn, scenario_pn, scenario_dir ),
	optimizer_pool( *m_Objective )
	{
		INIT_PROP( pn, prediction_window_, 500 );
		INIT_PROP( pn, prediction_start_, 100 );
		INIT_PROP( pn, prediction_look_ahead_, 1000 );
		INIT_PROP( pn, optimizations_, 6 );
		INIT_PROP( pn, concurrent_optimizations_, 3 );
		INIT_PROP( pn, random_seed_, 1 );
	}

	void CmaPoolOptimizer::Run()
	{
		// create output folder
		PrepareOutputFolder();

		// fill the pool
		for ( int i = 0; i < optimizations_; ++i )
		{
			// reuse the props from CmaPoolOptimizer
			props_.push_back( scenario_pn_copy_.get_child( "CmaPoolOptimizer" ) ); 
			props_.back().set( "random_seed", random_seed_ + i ); // new seed
			props_.back().set( "type", "CmaOptimizer" ); // change type
			props_.back().set( "output_root", GetOutputFolder() ); // make sure output is written to subdirectory
			props_.back().set( "log_level", xo::log::never_log_level ); // children don't log?
			push_back( std::make_unique< CmaOptimizerSpot >( props_.back(), scenario_pn_copy_, m_Objective->GetExternalResourceDir() ) );
		}

		// add reporters
		auto rep = add_new_reporter< spot::file_reporter >( GetOutputFolder() );
		rep->min_improvement_for_file_output = min_improvement_for_file_output;
		rep->max_steps_without_file_output = max_generations_without_file_output;
		add_new_reporter< CmaPoolOptimizerReporter >();

		// reset the id, so that the ProgressDock can interpret OutputStatus() as a general message
		id_.clear();

		run();
	}

	void CmaPoolOptimizer::SetOutputMode( OutputMode m )
	{
		output_mode_ = m;
		for ( auto& o : optimizers_ )
			dynamic_cast<Optimizer&>( *o ).SetOutputMode( m );
	}

	void CmaPoolOptimizerReporter::on_start( const optimizer& opt )
	{
		auto& cma = dynamic_cast<const CmaPoolOptimizer&>( opt );
		auto pn = cma.GetStatusPropNode();
		pn.set( "scenario", cma.GetOutputFolder().filename() );
		pn.set( "folder", cma.GetOutputFolder() );
		pn.set( "dim", cma.GetObjective().dim() );
		pn.set( "minimize", cma.IsMinimizing() );
		pn.set( "prediction_window", cma.prediction_window_ );
		cma.OutputStatus( pn );
	}

	void CmaPoolOptimizerReporter::on_stop( const optimizer& opt, const spot::stop_condition& s )
	{
		auto& cma = dynamic_cast<const CmaPoolOptimizer&>( opt );
		cma.OutputStatus( "finished", s.what() );
	}
}
