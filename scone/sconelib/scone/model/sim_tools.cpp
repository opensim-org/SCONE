#include "sim_tools.h"
#include "Sensors.h"

namespace scone
{
	namespace sim
	{
		SCONE_API Vec3 GetGroundCop( const Vec3& force, const Vec3& moment, Real min_force )
		{
			if ( force.y >= min_force )
				return Vec3( moment.z / force.y, 0, -moment.x / force.y );
			else return Vec3::zero();
		}

	}
}