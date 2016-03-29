

#include "Model.h"
#include "SensorDelayAdapter.h"
#include "Sensor.h"
#include "scone/core/core.h"
#include "scone/core/Storage.h"

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
				// special case because we need to add postfixes to the channel names
				m_StorageIdx = m_Model.GetSensorDelayStorage().AddChannel( source.GetName() + ".0" );
				for ( Index idx = 1; idx < source.GetChannelCount(); ++idx )
					m_Model.GetSensorDelayStorage().AddChannel( source.GetName() + "." + make_str( idx ) );
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

		scone::Real SensorDelayAdapter::GetValue( Index idx ) const
		{
			return GetValue( idx, m_Delay );
		}

		scone::Real SensorDelayAdapter::GetValue( Real delay ) const
		{
			return m_Model.GetSensorDelayStorage().GetInterpolatedValue( m_Model.GetTime() - delay * m_Model.sensor_delay_scaling_factor, m_StorageIdx );
		}

		scone::Real SensorDelayAdapter::GetValue( Index idx, Real delay ) const
		{
			return m_Model.GetSensorDelayStorage().GetInterpolatedValue( m_Model.GetTime() - delay * m_Model.sensor_delay_scaling_factor, m_StorageIdx + idx );
		}

		void SensorDelayAdapter::UpdateStorage()
		{
			Storage< Real >& storage = m_Model.GetSensorDelayStorage();
			SCONE_ASSERT( !storage.IsEmpty() && storage.Back().GetTime() == m_Model.GetTime() );

			// add the new value(s)
			for ( Index idx = 0; idx < GetChannelCount(); ++idx )
				storage.Back()[ m_StorageIdx + idx ] = m_InputSensor.GetValue( idx );
		}

		scone::Count SensorDelayAdapter::GetChannelCount()
		{
			return m_InputSensor.GetChannelCount();
		}

		scone::String SensorDelayAdapter::GetName() const
		{
			return m_InputSensor.GetName();
		}
	}
}
