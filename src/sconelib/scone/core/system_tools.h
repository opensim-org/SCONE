#pragma once

#include "core.h"
#include "PropNode.h"
#include "xo/system/version.h"
#include "xo/filesystem/path.h"

namespace scone
{
	using xo::version;
	using xo::path;
	SCONE_API version GetSconeVersion();
	SCONE_API String GetSconeBuildNumber();
	SCONE_API const PropNode& GetSconeSettings();
	SCONE_API void SaveSconeSettings( const PropNode& newSettings );

	enum SconeFolder { SCONE_ROOT_FOLDER, SCONE_RESULTS_FOLDER, SCONE_MODEL_FOLDER, SCONE_SCENARIO_FOLDER, SCONE_GEOMETRY_FOLDER, SCONE_UI_RESOURCE_FOLDER };
	SCONE_API path GetSettingsFolder();
	SCONE_API path GetInstallFolder();
	SCONE_API path GetDefaultDataFolder();
	//SCONE_API path GetFolder( const String& folder_name, const path& default_path = "" );
	SCONE_API path GetFolder( SconeFolder folder );
	SCONE_API path FindFile( const path& filename );
}
