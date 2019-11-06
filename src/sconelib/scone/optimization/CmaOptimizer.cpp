/*
** CmaOptimizer.cpp
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "CmaOptimizer.h"
#include "xo/string/string_tools.h"
#include "xo/filesystem/filesystem.h"

namespace scone
{
	const long DEFAULT_RANDOM_SEED = 123;

	CmaOptimizer::CmaOptimizer( const PropNode& props, const PropNode& scenario_pn, const path& scenario_dir ) :
		Optimizer( props, scenario_pn, scenario_dir ),
		mu_( 0 ),
		lambda_( 0 ),
		sigma_( 1.0 ),
		max_attempts( 100 )
	{
		INIT_PROP( props, lambda_, 0 );
		INIT_PROP( props, mu_, 0 );
		INIT_PROP( props, sigma_, 1.0 );
		INIT_PROP( props, window_size, 500 );
		INIT_PROP( props, random_seed, DEFAULT_RANDOM_SEED );
		INIT_PROP( props, flat_fitness_epsilon_, 1e-6 );
	}

	CmaOptimizer::~CmaOptimizer()
	{}

	String CmaOptimizer::GetClassSignature() const
	{
		auto str = Optimizer::GetClassSignature();
		if ( random_seed != DEFAULT_RANDOM_SEED )
			str += xo::stringf( ".R%d", random_seed );
		return str;
	}
}
