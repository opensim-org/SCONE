#include "stdafx.h"
#include "SensorDelayAdapter.h"
#include "Model.h"

namespace scone
{
	namespace sim
	{
		SensorDelayAdapter::SensorDelayAdapter( Model& model, Storage< Real >& storage, ChannelSensor& source, double delay ) :
		m_Model( model ),
		m_Storage( storage ),
		m_Source( source ),
		m_Delay( delay ),
		m_PreviousUpdateStep( -1 ),
		m_ChannelOfs( storage.GetChannelCount() )
		{
			// create channels in storage
			for ( size_t i = 0; i < source.GetSensorCount(); ++i )
				m_Storage.AddChannel( source.GetName() + "." + source.GetSensorName( i ) );
		}

		SensorDelayAdapter::~SensorDelayAdapter()
		{
		}

		Real SensorDelayAdapter::GetSensorValue( size_t idx ) const
		{
			return m_Storage.GetInterpolatedValue( m_Model.GetTime() - m_Delay, m_ChannelOfs + idx );
		}

		Real SensorDelayAdapter::GetSensorValue( size_t idx, TimeInSeconds delay ) const
		{
			//if ( m_ChannelOfs == 0 && idx == 1 )
			//{
			//	Real delayed_value = m_Storage.GetInterpolatedValue( m_Model.GetTime() - delay, m_ChannelOfs + idx );
			//	log::TraceF( "%.3f: current=%.3f delayed=%.3f delay=%g", m_Model.GetTime(), m_Source.GetSensorValue( idx ), delayed_value, delay );
			//}

			return m_Storage.GetInterpolatedValue( m_Model.GetTime() - delay, m_ChannelOfs + idx );
		}

		const String& SensorDelayAdapter::GetName() const 
		{
			return m_Source.GetName();
		}

		void SensorDelayAdapter::UpdateStorage()
		{
			SCONE_ASSERT( !m_Storage.IsEmpty() && m_Storage.Back().GetTime() == m_Model.GetTime() );

			// add the new values
			for ( size_t i = 0; i < m_Source.GetSensorCount(); ++i )
				m_Storage.Back()[ m_ChannelOfs + i ] = m_Source.GetSensorValue( i );
		}

		const StringIndexMap& SensorDelayAdapter::GetSensorNames() const 
		{
			return m_Source.GetSensorNames();
		}

	}
}
