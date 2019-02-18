/*
** Range.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "platform.h"
#include "PropNode.h"
#include "xo/numerical/math.h"

namespace scone
{
	template< typename T >
	class Range
	{
	public:
		Range() : min( T() ), max( T() ) {}
		Range( const PropNode& props ) : min( props.get< T >( "min", xo::constants<T>::lowest() ) ), max( props.get< T >( "max", xo::constants<T>::max() ) ) {}
		Range( const T& i_min, const T& i_max ) : min( i_min ), max( i_max ) {}

		// test if a value is inside the range
		bool Test( const T& value ) { return ( value >= min ) && ( value <= max ); }

		// get how much a value is outside the range
		// negative == below min, positive == above max, 0 = within range
		T GetRangeViolation( const T& value ) {
			if ( value < min ) return value - min;
			else if ( value > max ) return value - max;
			else return T( 0 );
		}

		T GetCenter() { return ( min + max ) / 2; }

		T min;
		T max;
	};

	typedef Range< Real > RealRange;
}
