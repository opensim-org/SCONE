#include "CmaOptimizer.h"
#include "flut/string_tools.hpp"

namespace scone
{
	const long DEFAULT_RANDOM_SEED = 123;

	CmaOptimizer::CmaOptimizer( const PropNode& props ) :
		Optimizer( props ),
		m_Mu( 0 ),
		m_Lambda( 0 ),
		m_Sigma( 1.0 ),
		max_generations( 10000 ),
		num_elitists( 0 ),
		max_attempts( 100 )
	{
		INIT_PROPERTY_NAMED( props, m_Lambda, "lambda", 0 );
		INIT_PROPERTY_NAMED( props, m_Mu, "mu", 0 );
		INIT_PROPERTY_NAMED( props, m_Sigma, "sigma", 1.0 );
		INIT_PROPERTY( props, max_generations, size_t( 3000 ) );
		INIT_PROPERTY( props, random_seed, DEFAULT_RANDOM_SEED );
		INIT_PROPERTY( props, global_std_factor, 0.0 );
		INIT_PROPERTY( props, global_std_offset, 0.0 );
		INIT_PROPERTY( props, use_init_file_std, true );
		INIT_PROPERTY( props, recombination_type, 2 );
	}

	scone::String CmaOptimizer::GetClassSignature() const
	{
		return Optimizer::GetClassSignature() + ( random_seed != DEFAULT_RANDOM_SEED ? flut::stringf( ".R%d", random_seed ) : "" );
	}

	CmaOptimizer::~CmaOptimizer()
	{
	}
}
