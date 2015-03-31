#include "stdafx.h"
#include "Log.h"

#include <boost/thread/mutex.hpp>

#define LOG_MESSAGE( LEVEL, FORMAT ) \
if ( LEVEL >= g_LogLevel ) \
{ \
	va_list args; va_start( args, FORMAT ); \
		char _buf_[ g_MaxLogMessageSize ]; \
		vsprintf_s( _buf_, sizeof( _buf_ ), FORMAT, args ); \
		LogMessage( LEVEL, _buf_ ); \
}

namespace scone
{
	namespace log
	{
		boost::mutex g_LogMutex;
		Level g_LogLevel = InfoLevel;
		const int g_MaxLogMessageSize = 1000;

		std::ostream& LogStream() { return std::cout; }

		void LogMessage( Level level, const char* message )
		{
			boost::lock_guard< boost::mutex > lock( g_LogMutex );
			LogStream() << message << std::endl;
		}
	
		void CORE_API log::SetLevel( Level level )
		{
			boost::lock_guard< boost::mutex > lock( g_LogMutex );
			g_LogLevel = level;	
		}

		void CORE_API WarningF( const char* msg, ... )
		{
			LOG_MESSAGE( WarningLevel, msg );
		}

		void CORE_API TraceF( const char* msg, ... )
		{
			LOG_MESSAGE( TraceLevel, msg );
		}

		void CORE_API DebugF( const char* msg, ... )
		{
			LOG_MESSAGE( DebugLevel, msg );
		}

		void CORE_API InfoF( const char* msg, ... )
		{
			LOG_MESSAGE( InfoLevel, msg );
		}

		void CORE_API ErrorF( const char* msg, ... )
		{
			LOG_MESSAGE( ErrorLevel, msg );
		}

		void CORE_API CriticalF( const char* msg, ... )
		{
			LOG_MESSAGE( CriticalLevel, msg );
		}
	}
}
