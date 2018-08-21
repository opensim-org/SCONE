#pragma once

#include "PropNode.h"
#include "Log.h"

namespace scone
{
	// log unused parameters
	SCONE_API void LogUntouched( const PropNode& p, log::Level level = log::WarningLevel, size_t depth = 0 );
}
