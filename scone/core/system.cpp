#include "stdafx.h"
#include "system.h"

#include <shlobj.h>
#include <fstream>
#include <boost/thread.hpp>
#include <boost/filesystem/path.hpp>

namespace scone
{
	boost::mutex g_SystemMutex;
	PropNode g_GlobalSettings;
	String g_Version;

	String GetLocalAppDataFolder()
	{
		// get the string
		wchar_t* wcsLocalAppData = 0;
		SHGetKnownFolderPath( FOLDERID_LocalAppData, 0, NULL, &wcsLocalAppData );

		// convert to single byte string
		char mbsLocalAppData[ MAX_PATH ];
		size_t dummy;
		wcstombs_s( &dummy, mbsLocalAppData, MAX_PATH, wcsLocalAppData, MAX_PATH );

		// free the string (ugh)
		CoTaskMemFree(static_cast<void*>( wcsLocalAppData ) );

		// convert to String and return
		return String( mbsLocalAppData );
	}

	CORE_API String GetApplicationFolder()
	{
		char buf[ 1024 ];
		GetModuleFileName( 0, buf, sizeof( buf ) );

		boost::filesystem::path folder( buf );
		return folder.parent_path().string();
	}

	const PropNode& GetSconeSettings()
	{
		boost::lock_guard< boost::mutex > lock( g_SystemMutex );

		// lazy initialization
		if ( g_GlobalSettings.IsEmpty() )
			g_GlobalSettings.FromIniFile( GetLocalAppDataFolder() + "/Scone/settings.ini" );

		return g_GlobalSettings;
	}

	CORE_API String GetSconeFolder( const String& folder )
	{
		return GetSconeSettings().GetStr( "folders." + folder ) + "/";
	}

	CORE_API String GetApplicationVersion()
	{
		if ( g_Version.empty() )
		{
			std::ifstream ifstr( GetApplicationFolder() + "/.version" );
			if ( ifstr.good() )
				ifstr >> g_Version;
			else g_Version = "UNKNOWN_VERSION";
		}

		return g_Version;
	}
}
