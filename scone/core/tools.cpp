#include "stdafx.h"
#include "tools.h"
#include <time.h>

using std::cout;
using std::endl;

namespace scone
{
	std::string GetStringF( const char* format, ... )
	{
		va_list args;
		va_start( args, format );
	
		char buf[8192];
		vsprintf_s(buf, sizeof(buf), format, args);
	
		return std::string(buf);
	}
	
	std::string GetDateTimeAsString()
	{
		__time64_t long_time;
		_time64(&long_time);
		struct tm t;
		_localtime64_s(&t, &long_time);
	
		char buf[100];
		sprintf_s(buf, sizeof(buf), "%02d%02d_%02d%02d", t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min);
	
		return buf;
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
						cout << " = " << iter->second->GetValue();
					cout << " *";
				}

				cout << endl;
			}

			LogUntouched( *iter->second, level, depth + 1 );
		}
	}
}
