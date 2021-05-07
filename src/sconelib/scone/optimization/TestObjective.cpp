/*
** TestObjective.cpp
**
** Copyright (C) 2013-2021 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "TestObjective.h"

#include "spot_test/test_functions.h"
#include "xo/string/string_tools.h"
#include "scone/core/string_tools.h"

namespace scone
{
	double schwefel( const spot::par_vec& v )
	{
		double sum = 0.0;
		for ( index_t i = 0; i < v.size(); ++i )
			sum += v[ i ] * sin( sqrt( fabs( v[ i ] ) ) );
		return 418.9829 * v.size() - sum;
	}

	TestObjective::TestObjective( const PropNode& pn, const path& find_file_folder ) :
	Objective( pn, find_file_folder )
	{
		INIT_PROP( pn, dim_, 10 );

		for ( index_t i = 0; i < dim_; ++i )
			info_.add( ParInfo( stringf( "P%d", i ), 0, 200, -500, 500 ) );
	}

	fitness_t TestObjective::evaluate( const SearchPoint& point ) const
	{
		return schwefel( point.values() );
	}

	String TestObjective::GetClassSignature() const
	{
		return stringf( "T%d", dim_ );
	}
}
