/*
** CmaPoolOptimizer.cpp
**
** Copyright (C) 2013-2021 Thomas Geijtenbeek and contributors. All rights reserved.
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
	optimizer_pool( *m_Objective, CmaOptimizerSpot::GetEvaluator(), pn )
	{
		// re-initialize these parameters because we want different defaults
		INIT_PROP( pn, prediction_window_, window_size );
		INIT_PROP( pn, prediction_start_, prediction_window_ );
		INIT_PROP( pn, prediction_look_ahead_, prediction_window_ );
		INIT_PROP( pn, max_generations_without_file_output, 100000 );

		INIT_PROP( pn, optimizations_, 6 );
		INIT_PROP( pn, active_optimizations_, 6 );
		INIT_PROP( pn, concurrent_optimizations_, 2 );
		INIT_PROP( pn, random_seed_, 1 );

		auto flag_parameters = CmaOptimizerSpot( pn, scenario_pn, scenario_dir );
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
			props_.back().set( "log_level", (int)xo::log::level::never ); // children don't log?

			// create optimizer
			auto o = std::make_unique< CmaOptimizerSpot >( props_.back(), scenario_pn_copy_, m_Objective->GetExternalResourceDir() );
			o->PrepareOutputFolder();
			o->add_reporter( std::make_unique< spot::file_reporter >(
				o->GetOutputFolder(), o->min_improvement_for_file_output, o->max_generations_without_file_output ) );
			o->SetOutputMode( output_mode_ );
			push_back( std::move( o ) );
		}

		// add reporters
		add_reporter( std::make_unique< spot::file_reporter >(
			GetOutputFolder(), min_improvement_for_file_output, max_generations_without_file_output ) );
		add_reporter( std::make_unique< CmaPoolOptimizerReporter >() );

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
		cma.OutputStatus( std::move( pn ) );
	}

	void CmaPoolOptimizerReporter::on_post_step( const optimizer& opt )
	{
		auto& pool = dynamic_cast<const CmaPoolOptimizer&>( opt );
		for ( const auto& o : pool.optimizers() )
		{
			auto& cma = dynamic_cast<const CmaOptimizer&>( *o );
			for ( auto&& pn : cma.GetStatusMessages() )
				pool.OutputStatus( std::move( pn ) );
		}
	}

	void CmaPoolOptimizerReporter::on_stop( const optimizer& opt, const spot::stop_condition& s )
	{
		auto& cma = dynamic_cast<const CmaPoolOptimizer&>( opt );
		cma.OutputStatus( "finished", s.what() );
	}
}
