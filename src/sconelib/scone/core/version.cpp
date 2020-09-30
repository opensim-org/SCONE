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
#include "system_tools.h"
#include "xo/string/string_cast.h"
#include <fstream>

constexpr int SCONE_VERSION_MAJOR = 1;
constexpr int SCONE_VERSION_MINOR = 6;
constexpr int SCONE_VERSION_PATCH = 0;
constexpr const char* SCONE_VERSION_POSTFIX = "ALPHA 1";

namespace scone
{
	int GetSconeBuildNumber()
	{
		xo::path versionpath( GetInstallFolder() / ".version" );
		int build = 0;
		if ( xo::exists( versionpath ) )
			xo::from_str( xo::load_string( versionpath ), build );
		return build;
	}

	const version& GetSconeVersion()
	{
		static version scone_version = version( SCONE_VERSION_MAJOR, SCONE_VERSION_MINOR, SCONE_VERSION_PATCH, GetSconeBuildNumber(), SCONE_VERSION_POSTFIX );
		return scone_version;
	}
}
