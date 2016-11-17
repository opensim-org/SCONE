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
#include "flut/system/path.hpp"
#include "Log.h"

#ifdef _MSC_VER
#include <shlobj.h>
#endif

#define SCONE_SETTINGS_PATH ( flut::get_config_folder() / "SCONE/settings.ini" )

namespace scone
{
	boost::mutex g_SystemMutex;
	PropNode g_GlobalSettings;
	String g_Version;
	path g_RootFolder;

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
		if ( g_GlobalSettings.IsEmpty() )
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

	SCONE_API path GetRootFolder()
	{
		if ( g_RootFolder.empty() )
		{
			for ( g_RootFolder = flut::get_application_folder(); !g_RootFolder.empty(); g_RootFolder = g_RootFolder.parent_path() )
			{
				if ( boost::filesystem::exists( boost::filesystem::path( g_RootFolder.str() ) / ".version" ) )
					break;
			}
			SCONE_THROW_IF( g_RootFolder.empty(), "Could not detect installation root folder" );
			log::debug( "SCONE root folder: ", g_RootFolder );
		}
		return g_RootFolder;
	}

	SCONE_API path GetDataFolder()
	{
		return flut::get_documents_folder() / "SCONE";
	}

	SCONE_API path GetFolder( const String& folder, const path& default_path )
	{
		if ( GetSconeSettings().HasKey( "folders" ) )
		{
			auto path_to_folder = flut::path( GetSconeSettings().GetChild( "folders" ).GetStr( folder, "" ) );
			if ( !path_to_folder.empty() )
				return path_to_folder;
		}
		return default_path;
	}

	SCONE_API path GetFolder( SconeFolder folder )
	{
		switch ( folder )
		{
		case scone::SCONE_ROOT_FOLDER: return GetRootFolder();
		case scone::SCONE_OUTPUT_FOLDER: return GetFolder( "output", GetDataFolder() / "models" );
		case scone::SCONE_MODEL_FOLDER: return GetFolder( "models", GetDataFolder() / "models" );
		case scone::SCONE_SCENARIO_FOLDER: return GetFolder( "scenarios", GetDataFolder() / "scenarios" );
		case scone::SCONE_GEOMETRY_FOLDER: return GetFolder( "geometry", GetRootFolder() / "resources/geometry" );
		case scone::SCONE_UI_RESOURCE_FOLDER: return GetFolder( "ui", GetRootFolder()/ "resources/ui" );
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
