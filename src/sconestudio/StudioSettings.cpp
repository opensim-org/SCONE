/*
** StudioSettings.cpp
**
** Copyright (C) 2013-2018 Thomas Geijtenbeek. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "StudioSettings.h"

#include "scone/core/system_tools.h"

namespace scone
{
	xo::settings& GetStudioSettings()
	{
		static auto s = xo::settings(
			load_file( GetInstallFolder() / "resources/studio-settings-schema.zml" ),
			GetSettingsFolder() / "studio-settings.zml"
		);
		return s;
	}
}
