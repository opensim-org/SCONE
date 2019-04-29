/*
** version.cpp
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "version.h"
#include "xo/filesystem/path.h"
#include "xo/filesystem/filesystem.h"
#include <fstream>

constexpr int SCONE_VERSION_MAJOR = 1;
constexpr int SCONE_VERSION_MINOR = 0;
constexpr int SCONE_VERSION_PATCH = 0;
constexpr const char* SCONE_VERSION_POSTFIX = SCONE_DEBUG ? "DEBUG" : "";

namespace scone
{
	int GetSconeBuildNumber()
	{
		xo::path versionpath( xo::get_application_folder() );
		int build = 0;

		// look for .version file, up to three levels from application folder
		for ( int level = 0; level <= 3; ++level )
		{
			if ( xo::exists( versionpath / ".version" ) )
			{
				// .version file found, read its contents
				std::ifstream ifstr( ( versionpath / ".version" ).str() );
				ifstr >> build;
				break;
			}
			else versionpath = versionpath / "..";
		}
		return build;
	}

	const version& GetSconeVersion()
	{
		static version scone_version = version( SCONE_VERSION_MAJOR, SCONE_VERSION_MINOR, SCONE_VERSION_PATCH, GetSconeBuildNumber(), SCONE_VERSION_POSTFIX );
		return scone_version;
	}
}
