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
	inline Vec3 ToVec3( const SimTK::Vec3& vec ) { return Vec3( vec[ 0 ], vec[ 1 ], vec[ 2 ] ); }
	inline Vec3f ToVec3f( const SimTK::Vec3& vec ) { return Vec3f( float( vec[ 0 ] ), float( vec[ 1 ] ), float( vec[ 2 ] ) ); }
	inline Vec3d ToVec3d( const SimTK::Vec3& vec ) { return Vec3d( vec[ 0 ], vec[ 1 ], vec[ 2 ] ); }

	inline SimTK::Vec3 make_osim( const Vec3& v ) { return SimTK::Vec3( v.x, v.y, v.z ); }
}
