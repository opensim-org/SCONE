#pragma once

#include "core.h"
#include "PropNode.h"
#include "flut/system/version.hpp"
#include "flut/system/path.hpp"

namespace scone
{
	using flut::version;
	using flut::path;
	SCONE_API version GetSconeVersion();
	SCONE_API String GetSconeBuildNumber();
	SCONE_API const PropNode& GetSconeSettings();
	SCONE_API void SaveSconeSettings( const PropNode& newSettings );

	enum SconeFolder { SCONE_ROOT_FOLDER, SCONE_RESULTS_FOLDER, SCONE_MODEL_FOLDER, SCONE_SCENARIO_FOLDER, SCONE_GEOMETRY_FOLDER, SCONE_UI_RESOURCE_FOLDER };
	SCONE_API path GetRootFolder();
	SCONE_API path GetDataFolder();
	SCONE_API path GetFolder( const String& folder_name, const path& default_path = "" );
	SCONE_API path GetFolder( SconeFolder folder );
}
