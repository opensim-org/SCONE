#include "CmaPoolOptimizer.h"
#include "CmaOptimizerSpot.h"

namespace scone
{
	CmaPoolOptimizer::CmaPoolOptimizer( const PropNode& pn ) :
	Optimizer( pn ),
	optimizer_pool( *m_Objective )
	{
		INIT_PROP( pn, prediction_window_size_, 200 );
		INIT_PROP( pn, prediction_start_, prediction_window_size_ );
		INIT_PROP( pn, max_concurrent_optimizations_, 200 );

		INIT_PROP( pn, random_seed_, 1 );
		INIT_PROP( pn, optimization_count_, 6 );

		for ( int i = 0; i < optimization_count_; ++i )
		{
			props_.push_back( pn ); // we're reusing the props from CmaPoolOptimizer
			props_.back().set( "random_seed", random_seed_ + i );
			props_.back().set( "type", "CmaOptimizer" );
			push_back( std::make_unique< CmaOptimizerSpot >( props_.back() ) );
		}
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
}
