#pragma once

#include "core.h"
#include "PropNode.h"

namespace scone
{
	CORE_API String GetLocalAppDataFolder();
	CORE_API String GetApplicationVersion();
	CORE_API const PropNode& GetSconeSettings();
	CORE_API String GetSconeFolder( const String& folder );
}
