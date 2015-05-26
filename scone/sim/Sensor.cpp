#include "stdafx.h"
#include "Sensor.h"

namespace scone
{
	namespace sim
	{
		Sensor::Sensor()
		{
		}

		Sensor::~Sensor()
		{
		}

		size_t Sensor::GetSensorCount() const
		{
			return GetSensorNames().GetSize();
		}

		const String& Sensor::GetSensorName( Index idx ) const
		{
			return GetSensorNames().GetString( idx );
		}

		Index Sensor::GetSensorIndex( const String& name ) const
		{
			return GetSensorNames().GetValue( name );
		}
	}
}
