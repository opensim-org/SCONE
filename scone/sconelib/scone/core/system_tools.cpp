#include "system_tools.h"

#include <fstream>

#include <boost/thread/mutex.hpp>
#include <boost/thread/lock_guard.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include "flut/system_tools.hpp"

#ifdef _MSC_VER
#include <shlobj.h>
#endif

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
		if ( g_GlobalSettings.IsEmpty() )
        	g_GlobalSettings.FromIniFile( flut::get_config_folder() + "/Scone/settings.ini" );

		return g_GlobalSettings;
	}

	SCONE_API String GetFolder( const String& folder, const String& default_path )
	{
		auto path_to_folder = GetSconeSettings().GetStr( "folders." + folder, "" );
		if ( path_to_folder.empty() )
			path_to_folder = GetSconeSettings().GetStr( "folders.root" ) + "/" + default_path;
		return path_to_folder + "/";
	}

	SCONE_API String GetFolder( SconeFolder folder )
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

	SCONE_API String GetApplicationVersion()
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
				g_Version = "UNKNOWN_VERSION"; // reading has failed, version unknown
		}

		return g_Version;
	}
}
