#include "stdafx.h"
#include "tools.h"
#include <time.h>

#ifdef WIN32
#include <shlwapi.h>
#pragma comment( lib, "shlwapi.lib" )
#else
#include <fnmatch.h>
#endif

#include <boost/date_time/posix_time/posix_time.hpp>
#include <sstream>

using std::cout;
using std::endl;
using namespace boost::posix_time;

namespace scone
{
	std::string GetStringF( const char* format, ... )
	{
		va_list args;
		va_start( args, format );
	
		char buf[8192];
		vsprintf_s(buf, sizeof(buf), format, args);

		va_end( args );
	
		return std::string(buf);
	}
	
	std::string GetDateTimeAsString()
	{
        time_facet *facet = new time_facet("%m%d.%H%M");
        static std::locale loc(std::cout.getloc(), facet);
        std::stringstream ss;
        ss.imbue(loc);
        ss << second_clock::local_time();
        	
		return ss.str();
	}

	std::string GetDateTimeExactAsString()
	{
        time_facet *facet = new time_facet("%m%d.%H%M%S");
        static std::locale loc(std::cout.getloc(), facet);
        std::stringstream ss;
        ss.imbue(loc);
        ss << second_clock::local_time();
        	
		return ss.str();
	}

	String CORE_API GetCleanVarName( const String& str )
	{
		return str.substr( 0, 2 ) == "m_" ? str.substr( 2 ) : str;
	}

	String CORE_API GetFilenameExt( const String& str )
	{
		size_t n = str.find_last_of(".");

		if (n == std::string::npos) 
			return std::string(""); // no extension found
		
		// dot could be part of a folder name
		if (str.substr(n).find_last_of("/\\") != std::string::npos)
			return std::string(""); // no extension found

		return str.substr(n);
	}

	String GetFileNameNoExt( const String& str )
	{
		size_t n = str.find_last_of(".");

		if ( n == std::string::npos )
			return str; // no extension found
		else return str.substr( 0, n );
	}

    // TODO: Could use regex to remove platform dependencies
    // Currently assumes one delimeter char. Can extend with boost if needed
	bool CORE_API MatchesPattern( const String& str, const String& pattern, bool multiple_patterns, char delim )
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

	void LogUntouched( const PropNode& p, log::Level level, size_t depth )
	{
		if ( depth == 0 )
		{
			size_t unused = p.GetUntouchedCount();
			if ( unused > 0 )
			{
				log::WarningF( "Warning, %d unused parameters found:", unused );
			}
			else return; // do nothing
		}

		// find more touched
		for ( PropNode::ConstChildIter iter = p.GetChildren().begin(); iter != p.GetChildren().end(); ++iter )
		{
			// find if any child is touched (err...)
			if ( iter->second->GetUntouchedCount() > 0 )
			{
				cout << String( depth * 2, ' ' ) << iter->first;

				if ( !iter->second->IsTouched() )
				{
					if ( iter->second->HasValue() )
						cout << " = " << iter->second->GetValueType();
					cout << " *";
				}

				cout << endl;
			}

			LogUntouched( *iter->second, level, depth + 1 );
		}
	}
}
