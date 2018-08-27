#include "TestObjective.h"

#include "test/test_functions.h"
#include "xo/string/string_tools.h"

namespace scone
{
	double schwefel( const spot::par_vec& v )
	{
		double sum = 0.0;
		for ( index_t i = 0; i < v.size(); ++i )
			sum += v[ i ] * sin( sqrt( fabs( v[ i ] ) ) );
		return 418.9829 * v.size() - sum;
	}

	TestObjective::TestObjective( const PropNode& pn ) :
	Objective( pn )
	{
		INIT_PROP( pn, dim_, 10 );

		for ( index_t i = 0; i < dim_; ++i )
			info_.add( xo::stringf( "P%d", i ), 0, 250, -500, 500 );
	}

	scone::fitness_t TestObjective::evaluate( const ParamInstance& point ) const
	{
		return schwefel( point.values() );
	}
}
