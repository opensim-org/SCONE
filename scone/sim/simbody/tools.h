#pragma once

#include "sim_simbody.h"
#include "..\..\core\Vec3.h"

#include <OpenSim/OpenSim.h>

namespace scone
{
	namespace sim
	{
		inline Vec3 ToVec3( SimTK::Vec3& vec ) { return Vec3( vec[ 0 ], vec[ 1 ], vec[ 2 ] ); }
	}
}