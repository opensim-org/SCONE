#pragma once

#include "core.h"
#include "PropNode.h"

namespace scone
{
	SCONE_API String GetApplicationVersion();
	SCONE_API const PropNode& GetSconeSettings();
	SCONE_API String GetSconeFolder( const String& folder );
}
