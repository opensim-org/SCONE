#include "stdafx.h"
#include "OptimizationTest.h"

namespace scone
{

	double ExampleObjective::Evaluate()
	{
		// Rosenbrock function
		double sum = 0.0;
		for( unsigned int i = 0; i < params.size()-1; i++ ) {
			sum += 100 * Square( params[i+1] - Square( params[ i ] ) ) + Square( 1. - params[ i ] );
		}

		return( sum );
	}

	void ExampleObjective::ProcessProperties( const PropNode& props )
	{
		PROCESS_PROPERTY( props, num_params );
		params.resize( num_params );
	}

	void ExampleObjective::ProcessParameters( opt::ParamSet& par )
	{
		for ( size_t i = 0; i < params.size(); ++i )
		{
			par.ProcessParameter( params[ i ], GetStringF( "Param%d", i), 0.5, 1.0, 0.0, 1.0 );
		}
	}

	void OptimizationTest()
	{
		opt::RegisterFactoryTypes();
		ExampleObjective::FactoryRegister();
		opt::Optimization opt;
		opt.Run("config/example_optimization.xml");
	}

}
