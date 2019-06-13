/*
** Log.cpp
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "Log.h"

#include <stdarg.h>
#include "xo/system/log_sink.h"
#include <iostream>

#ifdef WIN32
#	pragma warning( disable: 4996 ) // we don't need to push/pop because it's not a header
#endif

#define LOG_MESSAGE_F( LEVEL, FORMAT ) \
if ( xo::log::test_log_level( static_cast< xo::log::level >( LEVEL ) ) ) \
{ \
	va_list args; va_start( args, FORMAT ); \
		char _buf_[ g_MaxLogMessageSize ]; \
		vsnprintf( _buf_, sizeof( _buf_ ), FORMAT, args ); \
		LogMessageCheck( LEVEL, _buf_ ); \
		va_end( args ); \
}

namespace scone
{
	namespace log
	{
		// #todo: replace g_LogLevel with atomic variable
		// Even though simple read / write operations are atomic on WIN32
		// (see https://msdn.microsoft.com/en-us/library/windows/desktop/ms684122(v=vs.85).aspx)
		const int g_MaxLogMessageSize = 1000;

		std::ostream& LogStream() { return std::cout; }

		std::unique_ptr< xo::log::stream_sink > g_DefaultSink;

		void LogMessageCheck( Level level, const char* message )
		{
			xo::log::message( static_cast< xo::log::level >( level ), message );
		}

		void SCONE_API LogMessage( Level level, const String& msg )
		{
			LogMessageCheck( level, msg.c_str() );
		}

		void SCONE_API Trace( const String& msg )
		{
			LogMessageCheck( TraceLevel, msg.c_str() );
		}

		void SCONE_API Debug( const String& msg )
		{
			LogMessageCheck( DebugLevel, msg.c_str() );
		}

		void SCONE_API Info( const String& msg )
		{
			LogMessageCheck( InfoLevel, msg.c_str() );
		}

		void SCONE_API Warning( const String& msg )
		{
			LogMessageCheck( WarningLevel, msg.c_str() );
		}

		void SCONE_API Error( const String& msg )
		{
			LogMessageCheck( ErrorLevel, msg.c_str() );
		}

		void SCONE_API Critical( const String& msg )
		{
			LogMessageCheck( CriticalLevel, msg.c_str() );
		}

		void SCONE_API WarningF( const char* msg, ... )
		{
			LOG_MESSAGE_F( WarningLevel, msg );
		}

		void SCONE_API PeriodicTraceF( int period, const char* msg, ... )
		{
			static int counter = 0;
			if ( counter++ % period == 0 ) {
				LOG_MESSAGE_F( TraceLevel, msg );
			}
		}

		void SCONE_API TraceF( const char* msg, ... )
		{
			LOG_MESSAGE_F( TraceLevel, msg );
		}

		void SCONE_API DebugF( const char* msg, ... )
		{
			LOG_MESSAGE_F( DebugLevel, msg );
		}

		void SCONE_API InfoF( const char* msg, ... )
		{
			LOG_MESSAGE_F( InfoLevel, msg );
		}

		void SCONE_API ErrorF( const char* msg, ... )
		{
			LOG_MESSAGE_F( ErrorLevel, msg );
		}

		void SCONE_API CriticalF( const char* msg, ... )
		{
			LOG_MESSAGE_F( CriticalLevel, msg );
		}
	}
}
