#include "system_tools.h"

#define SCONE_VERSION_MAJOR 0
#define SCONE_VERSION_MINOR 9
#define SCONE_VERSION_PATCH 0
#define SCONE_VERSION_POSTFIX "ALPHA"

#include <fstream>

#include <boost/thread/mutex.hpp>
#include <boost/thread/lock_guard.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include "flut/system_tools.hpp"

#ifdef _MSC_VER
#include <shlobj.h>
#endif
#include "flut/system/path.hpp"
#include "Log.h"

#define SCONE_SETTINGS_PATH ( flut::get_config_folder() / "SCONE/settings.ini" )

namespace scone
{
	boost::mutex g_SystemMutex;
	PropNode g_GlobalSettings;
	String g_Version;

	SCONE_API String GetApplicationFolder()
	{
#ifdef _MSC_VER
		char buf[ 1024 ];
		GetModuleFileName( 0, buf, sizeof( buf ) );

		boost::filesystem::path folder( buf );
		return folder.parent_path().string();
#else
        return "";
#endif
	}

	const PropNode& GetSconeSettings()
	{
		boost::lock_guard< boost::mutex > lock( g_SystemMutex );

		// lazy initialization
		if ( g_GlobalSettings.empty() )
		{
			log::debug( "Loaded settings from ", SCONE_SETTINGS_PATH );
			g_GlobalSettings.FromIniFile( SCONE_SETTINGS_PATH.str() );
		}

		return g_GlobalSettings;
	}

	SCONE_API void SaveSconeSettings( const PropNode& newSettings )
	{
		boost::lock_guard< boost::mutex > lock( g_SystemMutex );
		g_GlobalSettings = newSettings;
		boost::filesystem::create_directories( boost::filesystem::path( SCONE_SETTINGS_PATH.str() ).parent_path() );
		g_GlobalSettings.ToIniFile( SCONE_SETTINGS_PATH.str() );
		log::debug( "Saved settings to ", SCONE_SETTINGS_PATH );
	}

	SCONE_API path GetFolder( const String& folder, const String& default_path )
	{
		auto path_to_folder = flut::path( GetSconeSettings().get_child( "folders" ).GetStr( folder, "" ) );
		if ( path_to_folder.empty() )
			path_to_folder = path( GetSconeSettings().get_child( "folders" ).GetStr( "root" ) ) / default_path;
		return path_to_folder;
	}

	SCONE_API path GetFolder( SconeFolder folder )
	{
		switch ( folder )
		{
		case scone::SCONE_ROOT_FOLDER: return GetFolder( "root" );
		case scone::SCONE_OUTPUT_FOLDER: return GetFolder( "output" );
		case scone::SCONE_MODEL_FOLDER: return GetFolder( "models", "models" );
		case scone::SCONE_SCENARIO_FOLDER: return GetFolder( "scenarios" );
		case scone::SCONE_GEOMETRY_FOLDER: return GetFolder( "geometry", "resources/geometry" );
		default: SCONE_THROW( "Unknown folder type" );
		}
	}

	SCONE_API flut::version GetSconeVersion()
	{
		auto build = GetSconeBuildNumber();
		int build_nr = 0;
		if ( build != "UNKNOWN" )
			build_nr = flut::from_str< int >( build );
		return version( SCONE_VERSION_MAJOR, SCONE_VERSION_MINOR, SCONE_VERSION_PATCH, build_nr, SCONE_VERSION_POSTFIX );

	}

	SCONE_API String GetSconeBuildNumber()
	{
		if ( g_Version.empty() )
		{
			boost::filesystem::path versionpath( GetApplicationFolder() );

			// look for .version file, up to three levels from application folder
			for ( int level = 0; level <= 3; ++level )
			{
				if ( boost::filesystem::exists( versionpath / ".version" ) )
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
