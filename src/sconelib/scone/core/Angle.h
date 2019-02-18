/*
** Angle.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "platform.h"
#include "xo/geometry/angle.h"
#include "types.h"

namespace scone
{
	// import Radian and Degree from xo
	using Degree = xo::degree_< Real >;
	using Radian = xo::radian_< Real >;
}
