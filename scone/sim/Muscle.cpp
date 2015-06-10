#include "stdafx.h"
#include "Muscle.h"

#include "../core/Profiler.h"

namespace scone
{
	namespace sim
	{
		StringIndexMap g_SensorNames = StringIndexMap(
			Muscle::MuscleLengthSensor, "MuscleLength", 
			Muscle::MuscleVelocitySensor, "MuscleVelocity", 
			Muscle::MuscleForceSensor, "MuscleForce" );

		Muscle::Muscle() :
		Actuator(),
		ChannelSensor()
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
			return g_SensorNames;
		}
	}
}
