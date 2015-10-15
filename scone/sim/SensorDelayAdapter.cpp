#include "stdafx.h"

#include "Model.h"
#include "SensorDelayAdapter.h"
#include "Sensor.h"
#include "../core/core.h"
#include "../core/Storage.h"

namespace scone
{
	namespace sim
	{
		SensorDelayAdapter::SensorDelayAdapter( Model& model, Sensor& source, TimeInSeconds default_delay ) :
		Sensor(),
		m_Model( model ),
		m_InputSensor( source ),
		m_Delay( default_delay )
		{
			if ( GetChannelCount() > 1 )
			{
				m_StorageIdx = m_Model.GetSensorDelayStorage().AddChannel( source.GetName() + ".0" );
				for ( Index idx = 1; idx < source.GetChannelCount(); ++idx )
					m_Model.GetSensorDelayStorage().AddChannel( source.GetName() + "." + ToString( idx ) );
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

		scone::Real SensorDelayAdapter::GetValue( Real delay ) const
		{
			return m_Model.GetSensorDelayStorage().GetInterpolatedValue( m_Model.GetTime() - delay * m_Model.sensor_delay_scaling_factor, m_StorageIdx );
		}

		Real* SensorDelayAdapter::GetValueArray( Real delay ) const
		{
			SCONE_THROW_NOT_IMPLEMENTED;
			//return m_Model.GetSensorDelayStorage().GetInterpolatedValue( m_Model.GetTime() - delay * m_Model.sensor_delay_scaling_factor, m_StorageIdx );
		}

		void SensorDelayAdapter::UpdateStorage()
		{
			Storage< Real >& storage = m_Model.GetSensorDelayStorage();
			SCONE_ASSERT( !storage.IsEmpty() && storage.Back().GetTime() == m_Model.GetTime() );

			// add the new value
			if ( GetChannelCount() > 1 )
			{
				for ( Index idx = 0; idx < GetChannelCount(); ++idx )
					storage.Back()[ m_StorageIdx + idx ] = m_InputSensor.GetValueArray()[ idx ];
			}
			else storage.Back()[ m_StorageIdx ] = m_InputSensor.GetValue();
		}

		scone::String SensorDelayAdapter::GetName() const 
		{
			return m_InputSensor.GetName();
		}
	}
}
