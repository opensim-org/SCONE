#pragma once

#include "core.h"
#include "PropNode.h"

namespace scone
{
	SCONE_API String GetApplicationVersion();
	SCONE_API const PropNode& GetSconeSettings();

	enum SconeFolder { SCONE_ROOT_FOLDER, SCONE_OUTPUT_FOLDER, SCONE_MODEL_FOLDER, SCONE_CONFIG_FOLDER, SCONE_GEOMETRY_FOLDER };
	SCONE_API String GetFolder( const String& folder_name, const String& default_path = "" );
	SCONE_API String GetFolder( SconeFolder folder );
}
