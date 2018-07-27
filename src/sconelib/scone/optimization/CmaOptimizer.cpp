#include "CmaOptimizer.h"
#include "xo/string/string_tools.h"

namespace scone
{
	const long DEFAULT_RANDOM_SEED = 123;

	CmaOptimizer::CmaOptimizer( const PropNode& props ) :
		Optimizer( props ),
		mu_( 0 ),
		lambda_( 0 ),
		sigma_( 1.0 ),
		num_elitists( 0 ),
		max_attempts( 100 )
	{
		INIT_PROPERTY( props, lambda_, 0 );
		INIT_PROPERTY( props, mu_, 0 );
		INIT_PROPERTY( props, sigma_, 1.0 );
		INIT_PROPERTY( props, window_size, size_t( 400 ) );
		INIT_PROPERTY( props, random_seed, DEFAULT_RANDOM_SEED );
	}

	scone::String CmaOptimizer::GetClassSignature() const
	{
		return Optimizer::GetClassSignature() + ( random_seed != DEFAULT_RANDOM_SEED ? xo::stringf( ".R%d", random_seed ) : "" );
	}

	CmaOptimizer::~CmaOptimizer()
	{
	}
}
