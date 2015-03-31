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
		void CORE_API TraceF( const char* msg, ... );
		void CORE_API DebugF( const char* msg, ... );
		void CORE_API InfoF( const char* msg, ... );
		void CORE_API WarningF( const char* msg, ... );
		void CORE_API ErrorF( const char* msg, ... );
		void CORE_API CriticalF( const char* msg, ... );
	}
}
