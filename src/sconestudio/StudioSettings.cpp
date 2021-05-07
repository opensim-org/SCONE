/*
** StudioSettings.cpp
**
** Copyright (C) 2013-2021 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "StudioSettings.h"

#include "scone/core/system_tools.h"
#include "scone/core/version.h"
#include "scone/core/Log.h"
#include "xo/serialization/serialize.h"
#include "xo/filesystem/filesystem.h"
#include "studio_config.h"

namespace scone
{
	StudioSettings::StudioSettings() :
		xo::settings(
			load_file( GetInstallFolder() / "resources/studio-settings-schema.zml" ),
			GetSettingsFolder() / "studio-settings.zml",
			GetSconeVersion()
		)
	{
		if ( data_version() < version( 0, 17, 0 ) )
		{
			log::warning( "Restoring studio settings to default" );
			reset(); // ignore settings from version < 0.17.0
		}

		// set defaults
		if ( get<path>( "gait_analysis.template" ).empty() )
			set<path>( "gait_analysis.template", GetFolder( SCONE_RESOURCE_FOLDER ).make_preferred() / "gaitanalysis/default.zml" );

		if ( get<path>( "video.path_to_ffmpeg" ).empty() )
			set<path>( "video.path_to_ffmpeg", xo::get_application_dir() / SCONE_FFMPEG_EXECUTABLE );
	}

	xo::settings& GetStudioSettings()
	{
		static StudioSettings settings;
		return settings;
	}
}
