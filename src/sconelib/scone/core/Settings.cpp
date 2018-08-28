#include "Settings.h"
#include "system_tools.h"
#include "xo/filesystem/filesystem.h"
#include "Log.h"
#include "../simvis/simvis/color.h"

namespace scone
{
	xo::settings& GetSconeSettings()
	{
		static auto settings = xo::settings(
			GetInstallFolder() / "resources/scone-settings-schema.zml",
			GetSettingsFolder() / "scone-settings.zml" );

		// set default paths if they don't exist
		if ( settings.get< path >( "folders.scenarios" ).empty() )
			settings.set( "folders.scenarios", GetDataFolder() );
		if ( settings.get< path >( "folders.results" ).empty() )
			settings.set( "folders.results", GetDataFolder() / "results" );
		if ( settings.get< path >( "folders.geometry" ).empty() )
			settings.set( "folders.geometry", GetInstallFolder() / "resources/geometry" );

		return settings;
	}

	void SaveSconeSettings()
	{
		auto settings_file = GetSettingsFolder() / "scone-settings.zml";
		xo::create_directories( xo::path( settings_file.str() ).parent_path() );
		GetSconeSettings().save( settings_file );
		log::debug( "Saved settings to ", settings_file );
	}
}
