/*
** StudioSettings.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "scone/core/platform.h"
#include "xo/system/settings.h"
#include "scone/core/types.h"
#include "scone/core/Exception.h"

namespace scone
{
	class StudioSettings : public xo::settings
	{
	public:
		StudioSettings();
	};

	xo::settings& GetStudioSettings();
	template< typename T > T GetStudioSetting( const String& key ) {
		try	{ return GetStudioSettings().get< T >( key ); }
		catch ( const std::exception& e ) { SCONE_ERROR( "Could not read setting \"" + key + "\" (" + e.what() + ")" ); }
	}
}
