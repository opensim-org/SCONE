/*
** Settings.cpp
**
** Copyright (C) 2013-2018 Thomas Geijtenbeek. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "Settings.h"
#include "system_tools.h"
#include "xo/filesystem/filesystem.h"
#include "Log.h"
#include "version.h"
#include "xo/serialization/serialize.h"

namespace scone
{
	u_ptr< xo::settings > scone_settings;

	xo::settings& GetSconeSettings()
	{
		if ( !scone_settings )
		{
			auto schema_path = GetInstallFolder() / "resources/scone-settings-schema.zml";
			auto settings_path = GetSettingsFolder() / "scone-settings.zml";
			scone_settings = std::make_unique< xo::settings >( load_file( schema_path ), settings_path, GetSconeVersion() );
			log::debug( "Loaded settings from ", settings_path );
		}

		// set default paths if they don't exist
		if ( scone_settings->get< path >( "folders.scenarios" ).empty() )
			scone_settings->set( "folders.scenarios", GetDataFolder() );
		if ( scone_settings->get< path >( "folders.results" ).empty() )
			scone_settings->set( "folders.results", GetDataFolder() / "results" );
		if ( scone_settings->get< path >( "folders.geometry" ).empty() )
			scone_settings->set( "folders.geometry", GetInstallFolder() / "resources/geometry" );

		return *scone_settings;
	}

	void SaveSconeSettings()
	{
		auto settings_file = GetSettingsFolder() / "scone-settings.zml";
		xo::create_directories( xo::path( settings_file.str() ).parent_path() );
		GetSconeSettings().save( settings_file );
		log::debug( "Saved settings to ", settings_file );
	}
}
