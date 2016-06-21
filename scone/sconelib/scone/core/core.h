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

#include <string>

// import FLUT logging, using only dynamic logging
#define FLUT_STATIC_LOG_LEVEL FLUT_LOG_LEVEL_TRACE
#define FLUT_DYNAMIC_LOG_LEVEL_FUNC scone::log::GetLevel

// enable / disable profiling
//#define SCONE_ENABLE_PROFILING

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
	typedef std::string String;

	// index type
	typedef size_t Index;
	typedef size_t Count;
	const Index NoIndex = size_t( -1 );
	typedef double TimeInSeconds;
}
