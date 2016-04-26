#pragma once

#include <iostream>
#include <iomanip>
#include "core.h"

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

		void CORE_API SetLevel( Level level );
		Level CORE_API GetLevel();
		void CORE_API LogMessage( Level level, const String& msg );

		// string style logging
		void CORE_API Trace( const String& msg );
		void CORE_API Debug( const String& msg );
		void CORE_API Info( const String& msg );
		void CORE_API Warning( const String& msg );
		void CORE_API Error( const String& msg );
		void CORE_API Critical( const String& msg );

		// printf style logging
		void CORE_API PeriodicTraceF( int period, const char* msg, ... );
		void CORE_API TraceF( const char* msg, ... );
		void CORE_API DebugF( const char* msg, ... );
		void CORE_API InfoF( const char* msg, ... );
		void CORE_API WarningF( const char* msg, ... );
		void CORE_API ErrorF( const char* msg, ... );
		void CORE_API CriticalF( const char* msg, ... );
	}
}
