#include "version.h"
#include "xo/filesystem/path.h"
#include "xo/filesystem/filesystem.h"
#include <fstream>

#define SCONE_VERSION_MAJOR 0
#define SCONE_VERSION_MINOR 15
#define SCONE_VERSION_PATCH 0
#define SCONE_VERSION_POSTFIX "BETA"

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
				std::ifstream ifstr( ( versionpath / ".version" ).string() );
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
