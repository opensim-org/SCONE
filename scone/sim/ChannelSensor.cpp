#include "stdafx.h"
#include "ChannelSensor.h"

namespace scone
{
	namespace sim
	{
		ChannelSensor::ChannelSensor()
		{
		}

		ChannelSensor::~ChannelSensor()
		{
		}

		size_t ChannelSensor::GetSensorCount() const
		{
			return GetSensorNames().GetSize();
		}

		const String& ChannelSensor::GetSensorName( Index idx ) const
		{
			return GetSensorNames().GetString( idx );
		}

		Index ChannelSensor::GetSensorIndex( const String& name ) const
		{
			return GetSensorNames().GetValue( name );
		}
	}
}
