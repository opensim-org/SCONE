#pragma once

#if defined(_MSC_VER)
#	ifdef SCONE_EXPORTS
#		define SCONE_API __declspec(dllexport)
#	else
#		define SCONE_API __declspec(dllimport)
#	endif
#	include <SDKDDKVer.h>
#	define NOMINMAX
#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>
#else
#	define SCONE_API
#endif

#include <limits>
#include <string>
#include <memory>

// import FLUT logging, using only dynamic logging
#define FLUT_STATIC_LOG_LEVEL FLUT_LOG_LEVEL_INFO
#define FLUT_DYNAMIC_LOG_LEVEL_FUNC scone::log::GetLevel

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
}
