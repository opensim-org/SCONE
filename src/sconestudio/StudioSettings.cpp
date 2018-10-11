/*
** StudioSettings.cpp
**
** Copyright (C) 2013-2018 Thomas Geijtenbeek. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "StudioSettings.h"

#include "scone/core/system_tools.h"
#include "scone/core/version.h"
#include "scone/core/Log.h"
#include <mutex>
#include "xo/serialization/serialize.h"

namespace scone
{
	xo::settings& GetStudioSettings()
	{
		static xo::settings s = xo::settings(
			load_file( GetInstallFolder() / "resources/studio-settings-schema.zml" ),
			GetSettingsFolder() / "studio-settings.zml",
			GetSconeVersion() );

		static std::once_flag flag;
		std::call_once( flag, [] {
			if ( s.data_version() < version( 0, 17, 0 ) )
			{
				log::info( "Restoring studio settings to default" );
				s.reset(); // ignore settings from version < 0.17.0
			}
		} );

		return s;
	}
}
