#pragma once

#include "xo/system/test_case.h"
#include "hfd/system/proxy_string.h"
#include <vector>

namespace scone
{
	void proxy_string_test( xo::test::test_case& XO_ACTIVE_TEST_CASE )
	{
		char test[ 80 ] = { 0 };
		const int strings = 10;
		const int length = 6;
		const int loops = 100;
		std::vector<hfd::proxy_string> indexes;
		indexes.resize( strings );
		for ( int attempt = 0; attempt < loops; ++attempt )
		{
			for ( int i = 0; i < strings; i++ )
			{
				for ( int j = 0; j < length; ++j )
					test[ j ] = 65 + i + j;
				indexes[ i ] = test;

				if ( attempt == loops - 1 )
					XO_CHECK( indexes[ i ] == string( test ) );
			}
		}
	}
}
