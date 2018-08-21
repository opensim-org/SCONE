#pragma once

#include "platform.h"
#include "types.h"

namespace scone
{
	namespace log
	{
		enum Level {
			LogAllLevel = 1,
			TraceLevel = 1,
			DebugLevel = 2,
			InfoLevel = 3,
			WarningLevel = 4,
			ErrorLevel = 5,
			CriticalLevel = 6,
			IgnoreAllLevel = 7
		};

		void SCONE_API LogMessage( Level level, const String& msg );

		// string style logging
		void SCONE_API Trace( const String& msg );
		void SCONE_API Debug( const String& msg );
		void SCONE_API Info( const String& msg );
		void SCONE_API Warning( const String& msg );
		void SCONE_API Error( const String& msg );
		void SCONE_API Critical( const String& msg );

		// printf style logging
		void SCONE_API PeriodicTraceF( int period, const char* msg, ... );
		void SCONE_API TraceF( const char* msg, ... );
		void SCONE_API DebugF( const char* msg, ... );
		void SCONE_API InfoF( const char* msg, ... );
		void SCONE_API WarningF( const char* msg, ... );
		void SCONE_API ErrorF( const char* msg, ... );
		void SCONE_API CriticalF( const char* msg, ... );
	}
}

// import XO logging, using dynamic logging
#define XO_STATIC_LOG_LEVEL XO_LOG_LEVEL_TRACE
#include "xo/system/log.h"

namespace scone
{
	namespace log
	{
		using xo::log::level;
		using xo::log::trace;
		using xo::log::debug;
		using xo::log::info;
		using xo::log::warning;
		using xo::log::error;
		using xo::log::critical;
	}
}
