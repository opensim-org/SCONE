#pragma once

#include <iostream>
#include <iomanip>
#include "core.h"

// very basic logging
#define SCONE_LOG( _MESSAGE_ ) { std::stringstream _STR_; _STR_ << _MESSAGE_; scone::Log( _STR_.str() ); }

namespace scone
{
	// thread safe logging
	void CORE_API Log( const String& message );
}
