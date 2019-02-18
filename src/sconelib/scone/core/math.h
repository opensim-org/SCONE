/*
** math.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "platform.h"
#include "types.h"

#include <limits>
#include <stdlib.h>
#include <cmath>

namespace scone
{
	// constants
	const Real REAL_PI = Real( 3.14159265358979323846 );
	const Real REAL_HALF_PI = Real( 1.57079632679489661923 );
	const Real REAL_EPSILON = std::numeric_limits< Real >::epsilon();
	const Real REAL_WIDE_EPSILON = 4 * std::numeric_limits< Real >::epsilon();
	const Real REAL_0 = Real( 0 );
	const Real REAL_1 = Real( 1 );
	const Real REAL_NAN = std::numeric_limits< Real >::quiet_NaN();
	const Real REAL_MIN = std::numeric_limits< Real >::min();
	const Real REAL_MAX = std::numeric_limits< Real >::max();
	const Real REAL_LOWEST = std::numeric_limits< Real >::lowest();

	// math helper functions
	inline Real DegToRad( Real d ) { return ( REAL_PI / 180 ) * d; }
	inline Real RadToDeg( Real d ) { return ( 180 / REAL_PI ) * d; }
	template< typename T > void Restrain( T& value, const T& min, const T& max )
	{ if ( value > max ) value = max; else if ( value < min ) value = min; }

	template< typename T > T GetRestrained( const T& value, const T& min, const T& max )
	{ if ( value > max ) return max; else if ( value < min ) return min; else return value; }

	template< typename T > bool IsWithinRange( const T& value, const T& min, const T& max )
	{ return ( value >= min ) && ( value <= max ); }

	// get uniform random value
	inline Real RandUni( Real lower, Real upper ) { return lower + ( upper - lower ) * rand() / RAND_MAX; }

	// sign function returns -1.0 on negative, 1.0 otherwise
	template< typename T > int Sign( const T& value ) { return (value < T(0)) ? -1 : 1; }

	// float comparison
	template< typename T >
	bool Equals( const T& v1, const T& v2, const T& tolerance = std::numeric_limits<T>::epsilon() ) { 
		return ( std::fabs( v1 - v2 ) <= std::fabs( v1 ) * tolerance );
	}

	// square function
	template< typename T > T GetSquared( T value ) { return value * value; }
	template< typename T > T GetCubed( T value ) { return value * value * value; }
}
