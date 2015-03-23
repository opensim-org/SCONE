#include "stdafx.h"
#include "system.h"

#include <shlobj.h>
#include <iosfwd>

namespace scone
{
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
}
