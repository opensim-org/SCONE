/*
** model_tools.cpp
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "model_tools.h"

#include "scone/core/string_tools.h"
#include "scone/core/profiler_config.h"
#include "Sensors.h"
#include "Muscle.h"
#include "Joint.h"
#include "Dof.h"
#include "Side.h"

using std::vector;
using std::pair;

namespace scone
{
	SCONE_API Vec3 GetGroundCop( const Vec3& force, const Vec3& moment, Real min_force )
	{
		if ( force.y >= min_force )
			return Vec3( moment.z / force.y, 0, -moment.x / force.y );
		else return Vec3::zero();
	}
}
