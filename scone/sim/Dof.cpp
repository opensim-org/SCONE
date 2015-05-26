#include "stdafx.h"
#include "Dof.h"

namespace scone
{
	namespace sim
	{
		StringMap< Index > Dof::m_SensorNames(
			DofPositionSensor, "DofPosition",
			DofVelocitySensor, "DofVelolicty" );

		Dof::Dof()
		{
		}

		Dof::~Dof()
		{
		}

		scone::Real Dof::GetSensorValue( Index idx ) const
		{
			// TODO: function pointers for better performance?
			switch( idx )
			{
			case DofPositionSensor: return GetPos();
			case DofVelocitySensor: return GetVel();
			default: SCONE_THROW( "Invalid sensor index" );
			}
		}

		const StringIndexMap& Dof::GetSensorNames() const
		{
			return m_SensorNames;
		}

	}
}
