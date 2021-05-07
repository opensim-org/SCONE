/*
** system_tools.h
**
** Copyright (C) 2013-2021 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "platform.h"
#include "xo/filesystem/path.h"

namespace scone
{
	using xo::path;

	enum SconeFolder { SCONE_ROOT_FOLDER, SCONE_RESULTS_FOLDER, SCONE_MODEL_FOLDER, SCONE_SCENARIO_FOLDER, SCONE_RESOURCE_FOLDER, SCONE_GEOMETRY_FOLDER, SCONE_UI_RESOURCE_FOLDER };
	SCONE_API path GetSettingsFolder();
	SCONE_API const path& GetInstallFolder();
	SCONE_API path GetDataFolder();
	SCONE_API path GetApplicationFolder();
	SCONE_API path GetFolder( SconeFolder folder );
	SCONE_API path FindFile( const path& filename );
}
