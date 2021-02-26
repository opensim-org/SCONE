/*
** version.h
**
** Copyright (C) 2013-2021 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "xo/system/version.h"
#include "platform.h"

namespace scone
{
	using xo::version;
	SCONE_API const version& GetSconeVersion();
}
