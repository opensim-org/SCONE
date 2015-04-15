#pragma once

namespace scone
{
	template< typename T >
	class Range
	{
	public:
		Range( const PropNode& props ) { min = props.Get< T >( "min" ); max = props.Get< T >( "max" ); }
		Range( const T& i_min = T( 0 ), const T& i_max = T( 0 ) ) : min( i_min ), max( i_max ) { };

		// test if a value is inside the range
		bool Test( const T& value ) { return ( value >= min ) && ( value <= max ); }

		// get how much a value is outside the range
		// negative == below min, positive == above max, 0 = within range
		double GetRangeViolation( const T& value ) {
			if ( value < min ) return value - min;
			else if ( value > max ) return value - max;
			else return 0.0;
		}

		T min;
		T max;
	};
}
