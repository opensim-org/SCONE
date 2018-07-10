#pragma once

#if defined(_MSC_VER)
#	ifdef SCONE_EXPORTS
#		define SCONE_API __declspec(dllexport)
#	else
#		define SCONE_API __declspec(dllimport)
#	endif
#else
#	define SCONE_API
#endif

#include <cstddef>
#include <string>
#include "xo/utility/types.h"

// enable / disable profiling
//#define SCONE_ENABLE_PROFILING
//#define SCONE_ENABLE_XO_PROFILING

#if defined(_MSC_VER)
// TODO: do this in a nicer way (i.e. push/pop warnings)
#pragma warning( disable: 4251 )
#endif

namespace scone
{
	// types
#ifdef SCONE_SINGLE_PRECISION_FLOAT
	typedef float Real;
#else
	typedef double Real;
#endif

	// index type
	using xo::index_t;
	typedef std::string String;
	using std::string;
	const index_t NoIndex = size_t( -1 );

	typedef double TimeInSeconds;
	typedef double Frequency;
}
