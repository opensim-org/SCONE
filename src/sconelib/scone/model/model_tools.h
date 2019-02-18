/*
** model_tools.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include <vector>
#include "scone/core/platform.h"
#include "scone/core/Vec3.h"
#include "scone/core/math.h"

namespace scone
{
	class Muscle;
	SCONE_API Vec3 GetGroundCop( const Vec3& force, const Vec3& moment, Real min_force = REAL_WIDE_EPSILON );
}
