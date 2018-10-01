/*
** StudioSettings.h
**
** Copyright (C) 2013-2018 Thomas Geijtenbeek. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "scone/core/platform.h"
#include "xo/system/settings.h"
#include "scone/core/types.h"

namespace scone
{
	xo::settings& GetStudioSettings();
	template< typename T > T GetStudioSetting( const String& key ) { return GetStudioSettings().get< T >( key ); }
}
