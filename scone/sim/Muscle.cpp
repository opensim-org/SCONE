#include "stdafx.h"
#include "Muscle.h"

#include "../core/Profiler.h"

namespace scone
{
	namespace sim
	{
		Muscle::Muscle() :
		Actuator()
		{
		}
		
		Muscle::~Muscle()
		{
		}

		MuscleForceSensor& Muscle::GetMuscleForceSensor()
		{
			if ( !m_pMuscleForceSensor )
				m_pMuscleForceSensor = std::unique_ptr< MuscleForceSensor >( new MuscleForceSensor( *this ) );
			return *m_pMuscleForceSensor;
		}

		MuscleLengthSensor& Muscle::GetMuscleLengthSensor()
		{
			if ( !m_pMuscleLengthSensor )
				m_pMuscleLengthSensor = std::unique_ptr< MuscleLengthSensor >( new MuscleLengthSensor( *this ) );
			return *m_pMuscleLengthSensor;
		}

		MuscleVelocitySensor& Muscle::GetMuscleVelocitySensor()
		{
			if ( !m_pMuscleVelocitySensor )
				m_pMuscleVelocitySensor = std::unique_ptr< MuscleVelocitySensor >( new MuscleVelocitySensor( *this ) );
			return *m_pMuscleVelocitySensor;
		}
	}
}
