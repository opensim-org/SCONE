/*
** Settings.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "platform.h"
#include "xo/system/settings.h"
#include "types.h"

namespace scone
{
	SCONE_API xo::settings& GetSconeSettings();
	SCONE_API void SaveSconeSettings();

	template< typename T > T GetSconeSetting( const String& key ) { return GetSconeSettings().get<T>( key ); }

	constexpr bool GetExperimentalFeaturesEnabled() { return SCONE_EXPERIMENTAL_FEATURES_ENABLED == 1; }
}
