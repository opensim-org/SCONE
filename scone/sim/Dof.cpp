#include "stdafx.h"
#include "Dof.h"

namespace scone
{
	namespace sim
	{
		StringMap< Index > Dof::m_SensorNames(
			DofSensorPosition, "DofSensorPosition",
			DofSensorVelocity, "DofSensorVelolicty" );

		Dof::Dof()
		{
		}

		Dof::~Dof()
		{
		}

		size_t Dof::GetSensorCount()
		{
			return DofSensorCount;
		}

		const String& Dof::GetSensorName( Index idx )
		{
			return m_SensorNames.GetString( idx );
		}

		scone::Real Dof::GetSensorValue( Index idx )
		{
			// TODO: function pointers for better performance?
			switch( idx )
			{
			case DofSensorPosition: return GetPos();
			case DofSensorVelocity: return GetVel();
			default: SCONE_THROW( "Invalid sensor index" );
			}
		}
	}
}
