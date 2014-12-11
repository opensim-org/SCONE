#include "stdafx.h"
#include "OptimizationTest.h"

double ExampleObjective::Evaluate()
{
	SCONE_THROW( "don't get here" );
	return Rosenbrock( params );
}

double ExampleObjective::Evaluate( opt::ParamSet& par )
{
	for ( int i = 0; i < 1000; ++i )
		Rosenbrock( par.GetFreeParamValues() );

	return Rosenbrock( par.GetFreeParamValues() );
}

void ExampleObjective::ProcessProperties( const PropNode& props )
{
	PROCESS_PROPERTY( props, num_params );
	params.resize( num_params );
}

void ExampleObjective::ProcessParameters( opt::ParamSet& par )
{
	for ( size_t i = 0; i < params.size(); ++i )
		par.ProcessParameter( params[ i ], GetStringF( "Param%d", i), 1.0, 0.1, -1000.0, 1000.0 );
}

double ExampleObjective::Rosenbrock( const std::vector< double >& v )
{
	// Rosenbrock function
	double sum = 0.0;
	for( unsigned int i = 0; i < v.size()-1; i++ ) {
		sum += 100 * Square( v[i+1] - Square( v[ i ] ) ) + Square( 1. - v[ i ] );
	}
	return( sum );
}

void OptimizationTest()
{
	ExampleObjective::RegisterFactory();
	opt::Optimization opt;
	opt.Run("config/example_optimization.xml");
}
