#include "ExampleObjective.h"
#include "scone/core/Exception.h"
#include "scone/core/PropNode.h"
#include "scone/core/InitFromPropNode.h"
#include "scone/opt/ParamSet.h"

namespace scone
{
	ExampleObjective::ExampleObjective( const PropNode& props, opt::ParamSet& par ) :
	Objective( props, par ),
	num_params( 0 ),
	is_evaluating( false )
	{
		INIT_PROPERTY( props, num_params, 0 );
		params.resize( num_params );
	}

	double ExampleObjective::Evaluate()
	{
		SCONE_ASSERT( is_evaluating == false ); // thread safety check

		is_evaluating = true; 
		double result = Rosenbrock( params );
		is_evaluating = false;

		return result;
	}

	void ExampleObjective::ProcessParameters( opt::ParamSet& par )
	{
		for ( size_t i = 0; i < params.size(); ++i )
			params[ i ] = par.GetMeanStd( stringf( "Param%d", i), 1.0, 0.1, -1000.0, 1000.0 );
	}

	double ExampleObjective::Rosenbrock( const std::vector< double >& v )
	{
		// Rosenbrock function
		double sum = 0.0;
		for( unsigned int i = 0; i < v.size()-1; i++ ) {
			sum += 100 * GetSquared( v[i+1] - GetSquared( v[ i ] ) ) + GetSquared( 1. - v[ i ] );
		}

		// Grind some
#ifdef _DEBUG
		int imax = 10000;
#else
		int imax = 10000000;
#endif
		for ( int i = 0; i < imax; ++i )
			sum += sqrt( (double)i ) - sqrt( (double)i );

		return( sum );
	}
}
