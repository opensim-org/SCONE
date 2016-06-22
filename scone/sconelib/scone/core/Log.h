#pragma once

#include "core.h"
#include "String.h"

// import FLUT logging, using only dynamic logging
#define FLUT_STATIC_LOG_LEVEL FLUT_LOG_LEVEL_TRACE
#define FLUT_DYNAMIC_LOG_LEVEL_FUNC scone::log::GetLevel
#include <flut/system/log.hpp>

namespace scone
{
	namespace log
	{
		enum Level {
			LogAllLevel = FLUT_LOG_LEVEL_ALL,
			TraceLevel = FLUT_LOG_LEVEL_TRACE,
			DebugLevel = FLUT_LOG_LEVEL_DEBUG,
			InfoLevel = FLUT_LOG_LEVEL_INFO,
			WarningLevel = FLUT_LOG_LEVEL_WARNING,
			ErrorLevel = FLUT_LOG_LEVEL_ERROR,
			CriticalLevel = FLUT_LOG_LEVEL_CRITICAL,
			IgnoreAllLevel = FLUT_LOG_LEVEL_NONE
		};

		// import flut logging
		using flut::log::trace;
		using flut::log::debug;
		using flut::log::info;
		using flut::log::warning;
		using flut::log::error;
		using flut::log::critical;

		void SCONE_API SetLevel( Level level );
		Level SCONE_API GetLevel();
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
