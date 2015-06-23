#include "stdafx.h"
#include "Dof.h"

namespace scone
{
	namespace sim
	{
		Dof::Dof()
		{
		}

		Dof::~Dof()
		{
		}

		DofPositionSensor& Dof::GetDofPositionSensor()
		{
			if ( !m_pPositionSensor )
				m_pPositionSensor = std::unique_ptr< DofPositionSensor >( new DofPositionSensor( *this ) );
			return *m_pPositionSensor;
		}

		DofVelocitySensor& Dof::GetDofVelocitySensor()
		{
			if ( !m_pVelocitySensor )
				m_pVelocitySensor = std::unique_ptr< DofVelocitySensor >( new DofVelocitySensor( *this ) );
			return *m_pVelocitySensor;
		}
	}
}
