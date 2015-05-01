#pragma once

namespace scone
{
	// math helper functions
	inline Real DegToRad( Real d ) { return ( REAL_PI / 180 ) * d; }
	inline Real RadToDeg( Real d ) { return ( 180 / REAL_PI ) * d; }
	template< typename T > void Restrain( T& value, const T& min, const T& max ) { if ( value > max ) value = max; else if ( value < min ) value = min; }
	template< typename T > T GetRestrained( const T& value, const T& min, const T& max ) {
		if ( value > max ) return max; else if ( value < min ) return min; else return value;
	}
	template< typename T > bool IsWithinRange( const T& value, const T& min, const T& max ) { return ( value >= min ) && ( value <= max ); }

	// get uniform random value
	inline Real RandUni( Real lower, Real upper ) { return lower + ( upper - lower ) * rand() / RAND_MAX; }

	// sign function returns -1.0 on negative, 1.0 otherwise
	template< typename T > T Sign( const T& value ) { return (value < T(0)) ? T(-1) : T(1); }

	// float comparison
	template< typename T >
	bool Equals( const T& v1, const T& v2, const T& tolerance = std::numeric_limits<T>::epsilon() ) { 
		return ( fabs( v1 - v2 ) <= fabs( v1 ) * tolerance );
	}

	// square function
	template< typename T > T GetSquared( T value ) { return value * value; }
	template< typename T > T GetCubed( T value ) { return value * value * value; }

	// Degree struct
	struct Rad;
	struct CORE_API Deg
	{
		Deg( Real v = 0.0 ) : value( v ) { };
		Deg( const Rad& v );
		operator Real&() { return value; }
		operator const Real&() const { return value; }

		friend std::ostream& operator<<( std::ostream& str, const Deg& v ) { return str << v.value; }
		friend std::istream& operator>>( std::istream& str, Deg& v ) { return str >> v.value; }

		Real value;
	};

	// Radian struct
	struct CORE_API Rad
	{
		Rad( Real v = 0.0 ) : value( v ) { };
		Rad( const Deg& v ) : value( v * Real( REAL_PI / 180.0 ) ) { };
		operator Real&() { return value; }
		operator const Real&() const { return value; }
		Real value;
	};

	inline Deg::Deg( const Rad& v ) : value( v * Real( 180.0 / REAL_PI ) ) { };
}
