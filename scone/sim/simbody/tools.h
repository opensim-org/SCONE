#pragma once

#include "sim_simbody.h"
#include "../../core/Vec3.h"

#include <OpenSim/OpenSim.h>
#include "../../core/ResourceCache.h"

namespace scone
{
	// opensim model resource cache
    template<> inline OpenSim::Model* ResourceCache< OpenSim::Model >::CreateFirst( const String& name ) 
    {
        return new OpenSim::Model( name );
    }
	template<> inline OpenSim::Storage* ResourceCache< OpenSim::Storage >::CreateFirst( const String& name )
    {
        return new OpenSim::Storage( name );
    };

	namespace sim
	{
		inline Vec3 ToVec3( const SimTK::Vec3& vec ) { return Vec3( vec[ 0 ], vec[ 1 ], vec[ 2 ] ); }
	}
}
