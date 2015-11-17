#pragma once

#include "sim_simbody.h"
#include "../../core/Vec3.h"

#include <OpenSim/OpenSim.h>
#include "../../core/ResourceCache.h"

namespace scone
{
	// opensim model resource cache
	template <> OpenSim::Model* ResourceCache< OpenSim::Model >::CreateFirst( const String& name );
	template <> OpenSim::Storage* ResourceCache< OpenSim::Storage >::CreateFirst( const String& name );

	namespace sim
	{
		inline Vec3 ToVec3( const SimTK::Vec3& vec ) { return Vec3( vec[ 0 ], vec[ 1 ], vec[ 2 ] ); }
	}
}
