#include "stdafx.h"
#include "ExampleObjective.h"
#include "../core/Exception.h"
#include "../core/PropNode.h"
#include "../core/Propertyable.h"
#include "../opt/ParamSet.h"

namespace scone
{
	double ExampleObjective::Evaluate()
	{
		SCONE_ASSERT( is_evaluating == false ); // thread safety check

		is_evaluating = true; 
		double result = Rosenbrock( params );
		is_evaluating = false;

		return result;
	}

	void ExampleObjective::ProcessProperties( const PropNode& props )
	{
		INIT_FROM_PROP( props, num_params, 0 );
		params.resize( num_params );
	}

	void ExampleObjective::ProcessParameters( opt::ParamSet& par )
	{
		for ( size_t i = 0; i < params.size(); ++i )
			params[ i ] = par( GetStringF( "Param%d", i), 1.0, 0.1, -1000.0, 1000.0 );
	}

	double ExampleObjective::Rosenbrock( const std::vector< double >& v )
	{
		// Rosenbrock function
		double sum = 0.0;
		for( unsigned int i = 0; i < v.size()-1; i++ ) {
			sum += 100 * Square( v[i+1] - Square( v[ i ] ) ) + Square( 1. - v[ i ] );
		}

		// Grind some
#ifdef _DEBUG
		int imax = 10000;
#else
		int imax = 10000000;
#endif
		for ( int i = 0; i < 10000000; ++i )
			sum += sqrt( (double)i ) - sqrt( (double)i );

		return( sum );
	}
}
