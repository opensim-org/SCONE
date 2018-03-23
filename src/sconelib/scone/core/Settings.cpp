#include "Settings.h"
#include "system_tools.h"
#include "xo/filesystem/filesystem.h"
#include "Log.h"
#include "../simvis/simvis/color.h"

namespace scone
{
	Settings g_Settings;

	const Settings& GetSconeSettings()
	{
		return g_Settings;
	}

	Settings& UpdateSconeSettings()
	{
		return g_Settings;
	}

	void SaveSconeSettings()
	{
		auto settings_file = GetSettingsFolder() / "settings.ini";
		xo::create_directories( xo::path( settings_file.str() ).parent_path() );

		g_Settings.save( settings_file );
		log::debug( "Saved settings to ", settings_file );
	}

	Settings::Settings()
	{
		// TODO: move to separate settings class
		add( "folders.scenarios", "scenario folder", GetDataFolder(), "Default location for SCONE scenarios" );
		add( "folders.results", "results folder", GetDataFolder() / "results", "Default location for optimization results" );
		add( "folders.geometry", "geometry folder", GetInstallFolder() / "resources/geometry", "Default location for model geometry" );

		add( "data.frequency", "data output frequency", 100.0, "Sample frequency of written simulation data" );
		add( "data.body", "Output body position and orientation", true, "Boolean indicating whether to output body position and orientation data (0 or 1)" );
		add( "data.muscle", "Output muscle parameters", true, "Boolean indicating whether to output muscle parameters (0 or 1)" );
		add( "data.sensor", "Output sensor data", true, "Boolean indicating whether to output sensor data (0 or 1)" );
		add( "data.controller", "Output controller data", true, "Boolean indicating whether to output controller data (0 or 1)" );

		auto settings_file = GetSettingsFolder() / "settings.ini";
		if ( xo::file_exists( settings_file ) )
		{
			log::debug( "Loaded settings from ", settings_file );
			g_Settings.load( settings_file );
		}
	}

	Settings::~Settings()
	{

	}
}
