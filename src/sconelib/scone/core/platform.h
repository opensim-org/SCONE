/*
** platform.h
**
** Copyright (C) 2013-2018 Thomas Geijtenbeek. All rights reserved.
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

#if defined(_MSC_VER)
// TODO: do this in a nicer way (i.e. push/pop warnings)
#pragma warning( disable: 4251 )
#endif
