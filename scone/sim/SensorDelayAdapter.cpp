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
		m_Source( source ),
		m_Delay( default_delay )
		{
			m_StorageIdx = m_Model.GetSensorDelayStorage().AddChannel( source.GetName() );
		}

		SensorDelayAdapter::~SensorDelayAdapter()
		{}

		scone::Real SensorDelayAdapter::GetValue() const 
		{
			return m_Model.GetSensorDelayStorage().GetInterpolatedValue( m_Model.GetTime() - m_Delay, m_StorageIdx );
		}

		scone::Real SensorDelayAdapter::GetValue( Real delay ) const
		{
			return m_Model.GetSensorDelayStorage().GetInterpolatedValue( m_Model.GetTime() - delay, m_StorageIdx );
		}

		void SensorDelayAdapter::UpdateStorage()
		{
			Storage< Real >& storage = m_Model.GetSensorDelayStorage();
			SCONE_ASSERT( !storage.IsEmpty() && storage.Back().GetTime() == m_Model.GetTime() );

			// add the new value
			storage.Back()[ m_StorageIdx ] = m_Source.GetValue();
		}

		scone::String SensorDelayAdapter::GetName() const 
		{
			return m_Source.GetName();
		}
	}
}
