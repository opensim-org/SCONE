#include "system_tools.h"

#include "scone/core/version.h"

#include <fstream>

#include "xo/system/system_tools.h"
#include "xo/filesystem/path.h"
#include "Log.h"

#ifdef _MSC_VER
#include <shlobj.h>
#endif
#include "xo/container/prop_node_tools.h"
#include "string"
#include "xo/utility/types.h"
#include "xo/filesystem/filesystem.h"
#include <mutex>

namespace scone
{
	std::mutex g_SystemMutex;
	xo::settings g_Settings;
	String g_Version;
	path g_RootFolder;

	void SetDefaultSconeSettings()
	{
		g_Settings.add( "folders.scenarios", "scenario folder", GetDataFolder(), "Default location for SCONE scenarios" );
		g_Settings.add( "folders.results", "results folder", GetDataFolder() / "results", "Default location for optimization results" );
		g_Settings.add( "folders.geometry", "geometry folder", GetInstallFolder() / "resources/geometry", "Default location for model geometry" );

		g_Settings.add( "data.frequency", "data output frequency", 100.0, "Sample frequency of written simulation data" );
		g_Settings.add( "data.body", "Output body position and orientation", true, "Boolean indicating whether to output body position and orientation data (0 or 1)" );
		g_Settings.add( "data.muscle", "Output muscle parameters", true, "Boolean indicating whether to output muscle parameters (0 or 1)" );
		g_Settings.add( "data.sensor", "Output sensor data", true, "Boolean indicating whether to output sensor data (0 or 1)" );
		g_Settings.add( "data.controller", "Output controller data", true, "Boolean indicating whether to output controller data (0 or 1)" );
	}

	const xo::settings& GetSconeSettings()
	{
		return UpdateSconeSettings();
	}

	xo::settings& UpdateSconeSettings()
	{
		std::lock_guard< std::mutex > lock( g_SystemMutex );
		if ( g_Settings.empty() )
		{
			SetDefaultSconeSettings();

			auto settings_file = GetSettingsFolder() / "settings.ini";
			if ( xo::file_exists( settings_file ) )
			{
				log::debug( "Loaded settings from ", settings_file );
				g_Settings.load( settings_file );
			}
		}

		return g_Settings;
	}

	void SaveSconeSettings()
	{
		std::lock_guard< std::mutex > lock( g_SystemMutex );
		auto settings_file = GetSettingsFolder() / "settings.ini";
		xo::create_directories( xo::path( settings_file.str() ).parent_path() );

		g_Settings.save( settings_file );
		log::debug( "Saved settings to ", settings_file );
	}

	path GetInstallFolder()
	{
		if ( g_RootFolder.empty() )
		{
			for ( g_RootFolder = xo::get_application_folder(); !g_RootFolder.empty(); g_RootFolder = g_RootFolder.parent_path() )
			{
				if ( xo::exists( g_RootFolder / ".version" ) )
					break;
			}
			SCONE_THROW_IF( g_RootFolder.empty(), "Could not detect installation root folder, please run .updateversion.bat" );
			log::debug( "SCONE root folder: ", g_RootFolder );
		}
		return g_RootFolder;
	}

	path GetFolder( const String& folder )
	{
		return GetSconeSettings().get< path >( "folders." + folder );
	}

	path GetSettingsFolder()
	{
		return xo::get_config_folder() / "SCONE";
	}

	path GetDataFolder()
	{
		return xo::get_documents_folder() / "SCONE";
	}

	path GetFolder( SconeFolder folder )
	{
		switch ( folder )
		{
		case scone::SCONE_ROOT_FOLDER: return GetInstallFolder();
		case scone::SCONE_RESULTS_FOLDER: return GetFolder( "results" );
		case scone::SCONE_SCENARIO_FOLDER: return GetFolder( "scenarios" );
		case scone::SCONE_GEOMETRY_FOLDER: return GetFolder( "geometry" );
		case scone::SCONE_UI_RESOURCE_FOLDER: return GetInstallFolder() / "resources/ui";
		default: SCONE_THROW( "Unknown folder type" );
		}
	}

	SCONE_API path FindFile( const path& p )
	{
		if ( file_exists( p ) )
			return p; // original path
		else if ( p.has_filename() && file_exists( p.filename() ) )
			return p.filename(); // filename in current folder
		else if ( p.has_filename() && file_exists( path( ".." ) / p.filename() ) )
			return path( ".." ) / p.filename(); // filename in folder one above current folder
		else SCONE_THROW( "Could not find " + p.string() + " in " + xo::current_path().string() );
	}

	xo::version GetSconeVersion()
	{
		auto build = GetSconeBuildNumber();
		int build_nr = 0;
		if ( build != "UNKNOWN" )
			build_nr = xo::from_str< int >( build );
		return version( SCONE_VERSION_MAJOR, SCONE_VERSION_MINOR, SCONE_VERSION_PATCH, build_nr, SCONE_VERSION_POSTFIX );

	}

	String GetSconeBuildNumber()
	{
		if ( g_Version.empty() )
		{
			xo::path versionpath( xo::get_application_folder() );

			// look for .version file, up to three levels from application folder
			for ( int level = 0; level <= 3; ++level )
			{
				if ( xo::exists( versionpath / ".version" ) )
				{
					// .version file found, read its contents
					std::ifstream ifstr( ( versionpath / ".version" ).string() );
					ifstr >> g_Version;
					break;

				}
				else versionpath = versionpath / "..";
			}

			if ( g_Version.empty() ) 
				g_Version = "UNKNOWN"; // reading has failed, version unknown
		}

		return g_Version;
	}
}
