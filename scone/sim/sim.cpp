#include "stdafx.h"
#include "sim.h"
#include "Sensors.h"
#include "Factories.h"

namespace scone
{
	namespace sim
	{
		void SCONE_SIM_API RegisterFactoryTypes()
		{
			GetSensorFactory().Register< MuscleForceSensor >();
			GetSensorFactory().Register< MuscleLengthSensor >();
			GetSensorFactory().Register< MuscleVelocitySensor >();
			GetSensorFactory().Register< MuscleSpindleSensor >();

			GetSensorFactory().Register< DofPositionSensor >();
			GetSensorFactory().Register< DofVelocitySensor >();
		}
	}
}