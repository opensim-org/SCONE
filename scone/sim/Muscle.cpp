#include "stdafx.h"
#include "Muscle.h"

#include "../core/Profiler.h"

namespace scone
{
	namespace sim
	{
		StringMap< Index > Muscle::m_SensorNames(
			Muscle::MuscleLengthSensor, "MuscleLength", 
			Muscle::MuscleVelocitySensor, "MuscleVelocity", 
			Muscle::MuscleForceSensor, "MuscleForce" );

		Muscle::Muscle() :
		Actuator(),
		Sensor()
		{
		}
		
		Muscle::~Muscle()
		{
		}

		scone::Real Muscle::GetSensorValue( Index idx ) const
		{
			SCONE_PROFILE_SCOPE;
			switch( idx )
			{
			case MuscleLengthSensor: return GetNormalizedFiberLength();
			case MuscleVelocitySensor: return GetNormalizedFiberVelocity();
			case MuscleForceSensor: return GetNormalizedForce();
			default: SCONE_THROW( "Invalid sensor index" );
			}
		}

		const StringIndexMap& Muscle::GetSensorNames() const 
		{
			return m_SensorNames;
		}
	}
}
