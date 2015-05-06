#include "stdafx.h"
#include "Muscle.h"

namespace scone
{
	namespace sim
	{
		StringMap< Index > Muscle::m_SensorNames(
			Muscle::MuscleLengthSensor, "MuscleLengthSensor", 
			Muscle::MuscleVelocitySensor, "MuscleVelocitySensor", 
			Muscle::MuscleForceSensor, "MuscleForceSensor" );

		Muscle::Muscle() :
		Actuator(),
		Sensor()
		{
		}
		
		Muscle::~Muscle()
		{
		}

		size_t Muscle::GetSensorCount()
		{
			return MuscleSensorCount;
		}

		const String& Muscle::GetSensorName( Index idx )
		{
			return m_SensorNames.GetString( idx );
		}

		scone::Real Muscle::GetSensorValue( Index idx )
		{
			switch( idx )
			{
			case MuscleLengthSensor: return GetNormalizedFiberLength();
			case MuscleVelocitySensor: return GetNormalizedFiberVelocity();
			case MuscleForceSensor: return GetNormalizedForce();
			default: SCONE_THROW( "Invalid sensor index" );
			}
		}
	}
}
