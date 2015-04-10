#pragma once

namespace scone
{
	template< typename T >
	class Range
	{
	public:
		Range( const PropNode& props ) { min = props.Get< T >( "min" ); max = props.Get< T >( "max" ); }
		Range( const T& i_min = T( 0 ), const T& i_max = T( 0 ) ) : min( i_min ), max( i_max ) { };
		bool Test( const T& value ) { return ( value >= min ) && ( value <= max ); }

		T min;
		T max;
	};
}
