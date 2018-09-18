#include "system_tools.h"

#include "Settings.h"

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
#include "Exception.h"

namespace scone
{
	std::mutex g_SystemMutex;
	String g_Version;
	path g_RootFolder;

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
}
