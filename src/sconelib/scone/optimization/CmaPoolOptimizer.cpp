#include "CmaPoolOptimizer.h"
#include "CmaOptimizerSpot.h"
#include "spot/file_reporter.h"

namespace scone
{
	CmaPoolOptimizer::CmaPoolOptimizer( const PropNode& pn ) :
	Optimizer( pn ),
	optimizer_pool( *m_Objective )
	{
		INIT_PROP( pn, prediction_window_, 200 );
		INIT_PROP( pn, prediction_start_, prediction_window_ );
		INIT_PROP( pn, prediction_look_ahead_, prediction_window_ );

		INIT_PROP( pn, random_seed_, 1 );
		INIT_PROP( pn, optimizations_, 6 );
		INIT_PROP( pn, concurrent_optimizations_, optimizations_ );

		// create output folder
		CreateOutputFolder( pn );

		// fill the pool
		for ( int i = 0; i < optimizations_; ++i )
		{
			props_.push_back( pn ); // we're reusing the props from CmaPoolOptimizer
			props_.back().set( "random_seed", random_seed_ + i );
			props_.back().set( "type", "CmaOptimizer" );
			props_.back().set( "output_root", GetOutputFolder() );
			props_.back().set( "log_level", xo::log::never_log_level );
			push_back( std::make_unique< CmaOptimizerSpot >( props_.back() ) );
		}

		// add reporters
		auto& rep = add_reporter< spot::file_reporter >( GetOutputFolder() );
		rep.min_improvement_factor_for_file_output = min_improvement_factor_for_file_output;
		rep.max_steps_without_file_output = max_generations_without_file_output;
		add_reporter< CmaPoolOptimizerReporter >();

		// reset the id, so that the ProgressDock can interpret OutputStatus() as a general message
		id_.clear();
	}

	void CmaPoolOptimizer::Run()
	{
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
