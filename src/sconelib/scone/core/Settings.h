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

namespace scone
{
	SCONE_API xo::settings& GetSconeSettings();
	SCONE_API void SaveSconeSettings();
}
