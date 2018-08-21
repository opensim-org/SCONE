#include "Model.h"
#include "SensorDelayAdapter.h"
#include "Sensor.h"
#include "scone/core/platform.h"
#include "scone/core/Storage.h"
#include "scone/core/string_tools.h"
#include <algorithm>

namespace scone
{
	SensorDelayAdapter::SensorDelayAdapter( Model& model, Sensor& source, TimeInSeconds default_delay ) :
		Sensor(),
		m_Model( model ),
		m_InputSensor( source ),
		m_Delay( default_delay )
	{
		if ( GetChannelCount() > 1 )
		{
			// special case because we need to add postfixes to the channel names
			m_StorageIdx = m_Model.GetSensorDelayStorage().AddChannel( source.GetName() + ".0" );
			for ( index_t idx = 1; idx < source.GetChannelCount(); ++idx )
				m_Model.GetSensorDelayStorage().AddChannel( source.GetName() + "." + to_str( idx ) );
		}
		else
		{
			m_StorageIdx = m_Model.GetSensorDelayStorage().AddChannel( source.GetName() );
		}
	}

	SensorDelayAdapter::~SensorDelayAdapter()
	{}

	scone::Real SensorDelayAdapter::GetValue() const
	{
		return GetValue( m_Delay );
	}

	scone::Real SensorDelayAdapter::GetValue( index_t idx ) const
	{
		return GetValue( m_StorageIdx + idx, m_Delay );
	}

	scone::Real SensorDelayAdapter::GetValue( Real delay ) const
	{
		return m_Model.GetSensorDelayStorage().GetInterpolatedValue( m_Model.GetTime() - delay * m_Model.sensor_delay_scaling_factor, m_StorageIdx );
	}

	scone::Real SensorDelayAdapter::GetValue( index_t idx, Real delay ) const
	{
		return m_Model.GetSensorDelayStorage().GetInterpolatedValue( m_Model.GetTime() - delay * m_Model.sensor_delay_scaling_factor, m_StorageIdx + idx );
	}

	scone::Real SensorDelayAdapter::GetAverageValue( int delay_samples, int window_size ) const
	{
		auto& sto = m_Model.GetSensorDelayStorage();
		auto history_begin = std::max( 0, (int)sto.GetFrameCount() - delay_samples - window_size / 2 );
		auto history_end = xo::clamped( (int)sto.GetFrameCount() - delay_samples - window_size / 2 + window_size, 1, (int)sto.GetFrameCount() );

		Real value = 0.0;
		for ( auto i = history_begin; i < history_end; ++i )
			value += sto.GetFrame( i )[ m_StorageIdx ];
		return value / ( history_end - history_begin );
	}

	void SensorDelayAdapter::UpdateStorage()
	{
		Storage< Real >& storage = m_Model.GetSensorDelayStorage();
		SCONE_ASSERT( !storage.IsEmpty() && storage.Back().GetTime() == m_Model.GetTime() );

		// add the new value(s)
		for ( index_t idx = 0; idx < GetChannelCount(); ++idx )
			storage.Back()[ m_StorageIdx + idx ] = m_InputSensor.GetValue( idx );
	}

	size_t SensorDelayAdapter::GetChannelCount()
	{
		return m_InputSensor.GetChannelCount();
	}

	scone::String SensorDelayAdapter::GetName() const
	{
		return m_InputSensor.GetName();
	}
}
