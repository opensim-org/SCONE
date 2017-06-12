#include "ExampleObjective.h"

#include "scone/core/Exception.h"
#include "scone/core/PropNode.h"
#include "scone/core/propnode_tools.h"
#include "scone/core/string_tools.h"
#include "scone/core/math.h"

#include "scone/optimization/Params.h"

namespace scone
{
	ExampleObjective::ExampleObjective( const PropNode& props ) :
	Objective( props ),
	num_params( 0 ),
	is_evaluating( false )
	{
		INIT_PROPERTY( props, num_params, 0 );
		for ( size_t i = 0; i < num_params; ++i )
			info().add( stringf( "Param%d", i ), 1.0, 0.1, -1000.0, 1000.0 );
	}

	double ExampleObjective::evaluate( const ParamInstance& values ) const
	{
		SCONE_ASSERT( is_evaluating == false ); // thread safety check

		is_evaluating = true;
		double result = Rosenbrock( values.values() );
		is_evaluating = false;

		return result;
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
