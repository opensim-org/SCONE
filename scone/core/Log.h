#pragma once

#include <iostream>
#include <iomanip>
#include "core.h"

// very basic logging
#define SCONE_LOG( _MESSAGE_ ) { std::stringstream _STR_; _STR_ << _MESSAGE_; scone::Log( _STR_.str() ); }

namespace scone
{
	namespace log
	{
		enum Level { LogAllLevel, TraceLevel, DebugLevel, InfoLevel, WarningLevel, ErrorLevel, CriticalLevel, IgnoreAllLevel };
		
		void CORE_API SetLevel( Level level );

		// printf style log messages
		void CORE_API Trace( const char* msg, ... );
		void CORE_API Debug( const char* msg, ... );
		void CORE_API Info( const char* msg, ... );
		void CORE_API Warning( const char* msg, ... );
		void CORE_API Error( const char* msg, ... );
		void CORE_API Critical( const char* msg, ... );
	}
}
