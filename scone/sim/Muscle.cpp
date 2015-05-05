#include "stdafx.h"
#include "Muscle.h"

namespace scone
{
	namespace sim
	{
		EnumStringMap< Muscle::MuscleSensor > Muscle::m_SensorNames = EnumStringMap< Muscle::MuscleSensor >(
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

		const String& Muscle::GetSensorName( size_t idx )
		{
			return m_SensorNames.GetString( static_cast< MuscleSensor >( idx ) );
		}

		scone::Real Muscle::GetSensorValue( size_t idx )
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
