/*
** simbody_tools.h
**
** Copyright (C) 2013-2018 Thomas Geijtenbeek. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "platform.h"

#include <OpenSim/OpenSim.h>

#include "scone/core/math.h"
#include "scone/core/Vec3.h"

namespace scone
{
	inline Vec3 from_osim( const SimTK::Vec3& vec ) { return Vec3( vec[ 0 ], vec[ 1 ], vec[ 2 ] ); }
	inline Quat from_osim( const SimTK::Quaternion& q ) { return Quat( q[ 0 ], q[ 1 ], q[ 2 ], q[ 3 ] ); }

	inline SimTK::Vec3 to_osim( const Vec3& v ) { return SimTK::Vec3( v.x, v.y, v.z ); }
}
