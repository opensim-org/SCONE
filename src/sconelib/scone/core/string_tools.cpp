/*
** string_tools.cpp
**
** Copyright (C) 2013-2018 Thomas Geijtenbeek. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "string_tools.h"

#include <time.h>

#ifdef WIN32
#include <shlwapi.h>
#pragma comment( lib, "shlwapi.lib" )
#pragma warning( disable: 4996 ) // no push/pop because it's a cpp file
#else
#include <fnmatch.h>
#endif

#include <sstream>
#include <cstdarg>
#include <chrono>
#include <iostream>
#include <ctime>

using std::cout;
using std::endl;

namespace scone
{
	std::string GetDateTimeAsString()
	{
		std::time_t today = std::chrono::system_clock::to_time_t( std::chrono::system_clock::now() );
		auto tm = std::localtime( &today );
		return stringf( "%02d%02d.%02d%02d", tm->tm_mon, tm->tm_mday, tm->tm_hour, tm->tm_min );
	}

	std::string GetDateTimeExactAsString()
	{
		std::time_t today = std::chrono::system_clock::to_time_t( std::chrono::system_clock::now() );
		auto tm = std::localtime( &today );
		auto p = std::chrono::high_resolution_clock::now();
		auto nsec = std::chrono::duration_cast<std::chrono::nanoseconds>( p.time_since_epoch() );
		auto frac_secs = nsec.count() % 1000000;
		return stringf( "%02d%02d.%02d%02d%02d.%06d", tm->tm_mon, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec, frac_secs );
	}

	// TODO: Could use regex to remove platform dependencies
	// Currently assumes one delimiter char.
	bool SCONE_API MatchesPattern( const String& str, const String& pattern, bool multiple_patterns, char delim )
	{
#ifdef WIN32
		return PathMatchSpecEx( str.c_str(), pattern.c_str(), multiple_patterns ? PMSF_MULTIPLE : PMSF_NORMAL ) == S_OK;
#else
		std::vector<std::string> tokens;
		std::stringstream ss(pattern);
		std::string token;
		while ( std::getline(ss, token, delim) ) {
			tokens.push_back(token);
		}
		for ( auto thisPattern : tokens ) {
			bool isMatch = fnmatch(thisPattern.c_str(), str.c_str(), FNM_NOESCAPE) == 0;
			if ( isMatch ) return isMatch;
		}
		return false;	
#endif
	}
}
