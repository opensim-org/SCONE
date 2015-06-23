#include "stdafx.h"
#include "Muscle.h"

#include "../core/Profiler.h"

namespace scone
{
	namespace sim
	{
		//StringIndexMap g_SensorNames = StringIndexMap(
		//	Muscle::MuscleLengthSensor, "MuscleLength", 
		//	Muscle::MuscleVelocitySensor, "MuscleVelocity", 
		//	Muscle::MuscleForceSensor, "MuscleForce" );

		Muscle::Muscle() :
		Actuator()
		{
		}
		
		Muscle::~Muscle()
		{
		}

		MuscleForceSensor& Muscle::GetForceSensor()
		{
			if ( !m_pMuscleForceSensor )
				m_pMuscleForceSensor = std::unique_ptr< MuscleForceSensor >( new MuscleForceSensor( *this ) );
			return *m_pMuscleForceSensor;
		}

		MuscleLengthSensor& Muscle::GetLengthSensor()
		{
			if ( !m_pMuscleLengthSensor )
				m_pMuscleLengthSensor = std::unique_ptr< MuscleLengthSensor >( new MuscleLengthSensor( *this ) );
			return *m_pMuscleLengthSensor;
		}

		MuscleVelocitySensor& Muscle::GetVelocitySensor()
		{
			if ( !m_pMuscleVelocitySensor )
				m_pMuscleVelocitySensor = std::unique_ptr< MuscleVelocitySensor >( new MuscleVelocitySensor( *this ) );
			return *m_pMuscleVelocitySensor;
		}

		//scone::Real Muscle::GetSensorValue( Index idx ) const
		//{
		//	SCONE_PROFILE_SCOPE;
		//	switch( idx )
		//	{
		//	case MuscleLengthSensor: return GetNormalizedFiberLength();
		//	case MuscleVelocitySensor: return GetNormalizedFiberVelocity();
		//	case MuscleForceSensor: return GetNormalizedForce();
		//	default: SCONE_THROW( "Invalid sensor index" );
		//	}
		//}

		//const StringIndexMap& Muscle::GetSensorNames() const 
		//{
		//	return g_SensorNames;
		//}
	}
}
