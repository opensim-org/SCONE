#include "sim_tools.h"
#include "Sensors.h"
#include "Factories.h"

namespace scone
{
	namespace sim
	{
		void SCONE_API RegisterFactoryTypes()
		{
			GetSensorFactory().Register< MuscleForceSensor >();
			GetSensorFactory().Register< MuscleLengthSensor >();
			GetSensorFactory().Register< MuscleVelocitySensor >();
			GetSensorFactory().Register< MuscleSpindleSensor >();

			GetSensorFactory().Register< DofPositionSensor >();
			GetSensorFactory().Register< DofVelocitySensor >();
		}

		SCONE_API Vec3 GetGroundCop( const Vec3& force, const Vec3& moment, Real min_force )
		{
			if ( force.y >= min_force )
				return Vec3( moment.z / force.y, 0, -moment.x / force.y );
			else return Vec3::zero();
		}

	}
}