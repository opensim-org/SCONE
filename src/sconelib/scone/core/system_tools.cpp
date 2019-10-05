/*
** system_tools.cpp
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "system_tools.h"

#include "xo/system/system_tools.h"
#include "xo/filesystem/path.h"
#include "xo/container/prop_node_tools.h"
#include "xo/xo_types.h"
#include "xo/filesystem/filesystem.h"

#include "Settings.h"
#include "Log.h"
#include "Exception.h"

#include <mutex>

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
			SCONE_THROW_IF( g_RootFolder.empty(), "Could not detect installation root folder, please run .updateversion.bat or .updateversion.sh" );
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

	path FindFile( const path& p )
	{
		return xo::find_file( {
			p,
			p.filename(), // filename without rel path
			path( ".." ) / p.filename() // filename in parent folder
			} );
	}
}
