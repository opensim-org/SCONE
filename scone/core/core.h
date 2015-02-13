#pragma once

#include <limits>
#include <string>

#ifdef CORE_EXPORTS
#define CORE_API __declspec(dllexport)
#else
#define CORE_API __declspec(dllimport)
#endif

// TODO: do this in a nicer way (i.e. push/pop warnings)
#pragma warning( disable: 4251 )

// class and shared pointer forward declaration macro
#define SCONE_DECLARE_CLASS_AND_PTR( _class_ ) \
	class _class_; \
	typedef std::shared_ptr< _class_ > _class_##SP; \
	typedef std::unique_ptr< _class_ > _class_##UP;

namespace scone
{
	// types
#ifdef SCONE_SINGLE_PRECISION_FLOAT
	typedef float Real;
#else
	typedef double Real;
#endif
	typedef std::string String;
	typedef size_t Index;

	// constants
	const Real REAL_PI = Real( 3.14159265358979323846 );
	const Real REAL_HALF_PI = Real( 1.57079632679489661923 );
	const Real REAL_EPSILON = std::numeric_limits< Real >::epsilon();
	const Real REAL_0 = Real( 0 );
	const Real REAL_1 = Real( 1 );
	const Real REAL_NAN = std::numeric_limits< Real >::quiet_NaN();
	const Real REAL_MIN = std::numeric_limits< Real >::min();
	const Real REAL_MAX = std::numeric_limits< Real >::max();
	const Index NO_INDEX = std::numeric_limits< Index >::max();

	// forward declarations
	class Vec3;
	class Quat;
}
