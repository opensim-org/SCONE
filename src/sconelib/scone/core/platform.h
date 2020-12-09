/*
** platform.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

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

// enable / disable profiling
//#define SCONE_ENABLE_PROFILING
//#define SCONE_ENABLE_XO_PROFILING

#ifdef NDEBUG
constexpr bool SCONE_DEBUG = false;
#else
constexpr bool SCONE_DEBUG = true;
#endif

#ifdef SCONE_EXPERIMENTAL_FEATURES
#	define SCONE_EXPERIMENTAL_FEATURES_ENABLED 1
#else
#	define SCONE_EXPERIMENTAL_FEATURES_ENABLED 0
#endif

#if defined(_MSC_VER)
#	pragma warning( disable: 4251 ) // disable W4251, unfortunately there's no nice way to do this
#endif
