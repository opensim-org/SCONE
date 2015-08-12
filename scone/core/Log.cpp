#include "stdafx.h"
#include "Log.h"

#include <boost/thread/mutex.hpp>
#include <boost/thread/lock_guard.hpp>

#define LOG_MESSAGE_F( LEVEL, FORMAT ) \
if ( LEVEL >= g_LogLevel ) \
{ \
	va_list args; va_start( args, FORMAT ); \
		char _buf_[ g_MaxLogMessageSize ]; \
		vsprintf_s( _buf_, sizeof( _buf_ ), FORMAT, args ); \
		LogMessageNoCheck( LEVEL, _buf_ ); \
		va_end( args ); \
}

namespace scone
{
	namespace log
	{
		// TODO: replace g_LogLevel with atomic variable
		// Even though simple read / write operations are atomic on WIN32
		// (see https://msdn.microsoft.com/en-us/library/windows/desktop/ms684122(v=vs.85).aspx)
		Level g_LogLevel = InfoLevel;
		boost::mutex g_LogMutex;
		const int g_MaxLogMessageSize = 1000;

		std::ostream& LogStream() { return std::cout; }

		void LogMessageNoCheck( Level level, const char* message )
		{
			boost::lock_guard< boost::mutex > lock( g_LogMutex );
			LogStream() << message << std::endl;
		}

		void LogMessageCStr( Level level, const char* message )
		{
			if ( level >= g_LogLevel )
				LogMessageNoCheck( level, message );
		}

		void CORE_API LogMessage( Level level, const String& msg )
		{
			LogMessageCStr( level, msg.c_str() );
		}

		void CORE_API SetLevel( Level level )
		{
			g_LogLevel = level;	
		}

		void CORE_API Trace( const String& msg )
		{
			LogMessageCStr( TraceLevel, msg.c_str() );
		}

		void CORE_API Debug( const String& msg )
		{
			LogMessageCStr( DebugLevel, msg.c_str() );
		}

		void CORE_API Info( const String& msg )
		{
			LogMessageCStr( InfoLevel, msg.c_str() );
		}

		void CORE_API Warning( const String& msg )
		{
			LogMessageCStr( WarningLevel, msg.c_str() );
		}

		void CORE_API Error( const String& msg )
		{
			LogMessageCStr( ErrorLevel, msg.c_str() );
		}

		void CORE_API Critical( const String& msg )
		{
			LogMessageCStr( CriticalLevel, msg.c_str() );
		}

		void CORE_API WarningF( const char* msg, ... )
		{
			LOG_MESSAGE_F( WarningLevel, msg );
		}

		void CORE_API TraceF( const char* msg, ... )
		{
			LOG_MESSAGE_F( TraceLevel, msg );
		}

		void CORE_API DebugF( const char* msg, ... )
		{
			LOG_MESSAGE_F( DebugLevel, msg );
		}

		void CORE_API InfoF( const char* msg, ... )
		{
			LOG_MESSAGE_F( InfoLevel, msg );
		}

		void CORE_API ErrorF( const char* msg, ... )
		{
			LOG_MESSAGE_F( ErrorLevel, msg );
		}

		void CORE_API CriticalF( const char* msg, ... )
		{
			LOG_MESSAGE_F( CriticalLevel, msg );
		}
}
}
