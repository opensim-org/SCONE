#pragma once

#if defined(_MSC_VER)

#ifdef CORE_EXPORTS
#define CORE_API __declspec(dllexport)
#else
#define CORE_API __declspec(dllimport)
#endif

#else

#define CORE_API

#endif

#include <limits>
#include <string>
#include <memory>

// enable / disable profiling
//#define SCONE_ENABLE_PROFILING

#if defined(_MSC_VER)
// TODO: do this in a nicer way (i.e. push/pop warnings)
#pragma warning( disable: 4251 )
#endif

// class and shared pointer forward declaration macro
#define SCONE_DECLARE_CLASS_AND_PTR( _class_ ) \
	class _class_; \
	typedef std::shared_ptr< _class_ > _class_##SP; \
	typedef std::unique_ptr< _class_ > _class_##UP;

// class and shared pointer forward declaration macro
#define SCONE_DECLARE_STRUCT_AND_PTR( _class_ ) \
struct _class_; \
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

	// index type
	typedef size_t Index;
	typedef size_t Count;
	const Index NoIndex = std::numeric_limits< Index >::max();
	typedef double TimeInSeconds;

	// constants
	const Real REAL_PI = Real( 3.14159265358979323846 );
	const Real REAL_HALF_PI = Real( 1.57079632679489661923 );
	const Real REAL_EPSILON = std::numeric_limits< Real >::epsilon();
	const Real REAL_0 = Real( 0 );
	const Real REAL_1 = Real( 1 );
	const Real REAL_NAN = std::numeric_limits< Real >::quiet_NaN();
	const Real REAL_MIN = std::numeric_limits< Real >::min();
	const Real REAL_MAX = std::numeric_limits< Real >::max();
	const Real REAL_LOWEST = std::numeric_limits< Real >::lowest();

	// forward declarations
	class Vec3;
	class Quat;
}
