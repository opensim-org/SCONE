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
		max_generations( 10000 ),
		num_elitists( 0 ),
		max_attempts( 100 )
	{
		INIT_PROPERTY( props, lambda_, 0 );
		INIT_PROPERTY( props, mu_, 0 );
		INIT_PROPERTY( props, sigma_, 1.0 );
		INIT_PROPERTY( props, max_generations, size_t( 3000 ) );
		INIT_PROPERTY( props, window_size, size_t( 500 ) );
		INIT_PROPERTY( props, random_seed, DEFAULT_RANDOM_SEED );
		INIT_PROPERTY( props, global_std_factor, 0.0 );
		INIT_PROPERTY( props, global_std_offset, 0.0 );
		INIT_PROPERTY( props, init_file_std_factor, 1.0 );
		INIT_PROPERTY( props, init_file_std_offset, 0.0 );
		INIT_PROPERTY( props, use_init_file_std, true );
		INIT_PROPERTY( props, recombination_type, 2 );
	}

	scone::String CmaOptimizer::GetClassSignature() const
	{
		return Optimizer::GetClassSignature() + ( random_seed != DEFAULT_RANDOM_SEED ? xo::stringf( ".R%d", random_seed ) : "" );
	}

	CmaOptimizer::~CmaOptimizer()
	{
	}
}
