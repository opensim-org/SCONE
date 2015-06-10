#include "stdafx.h"
#include "ChannelSensorDelayAdapter.h"
#include "Model.h"

namespace scone
{
	namespace sim
	{
		ChannelSensorDelayAdapter::ChannelSensorDelayAdapter( Model& model, Storage< Real >& storage, ChannelSensor& source, double delay ) :
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

		ChannelSensorDelayAdapter::~ChannelSensorDelayAdapter()
		{
		}

		Real ChannelSensorDelayAdapter::GetSensorValue( size_t idx ) const
		{
			return m_Storage.GetInterpolatedValue( m_Model.GetTime() - m_Delay, m_ChannelOfs + idx );
		}

		Real ChannelSensorDelayAdapter::GetSensorValue( size_t idx, TimeInSeconds delay ) const
		{
			//if ( m_ChannelOfs == 0 && idx == 1 )
			//{
			//	Real delayed_value = m_Storage.GetInterpolatedValue( m_Model.GetTime() - delay, m_ChannelOfs + idx );
			//	log::TraceF( "%.3f: current=%.3f delayed=%.3f delay=%g", m_Model.GetTime(), m_Source.GetSensorValue( idx ), delayed_value, delay );
			//}

			return m_Storage.GetInterpolatedValue( m_Model.GetTime() - delay, m_ChannelOfs + idx );
		}

		const String& ChannelSensorDelayAdapter::GetName() const 
		{
			return m_Source.GetName();
		}

		void ChannelSensorDelayAdapter::UpdateStorage()
		{
			SCONE_ASSERT( !m_Storage.IsEmpty() && m_Storage.Back().GetTime() == m_Model.GetTime() );

			// add the new values
			for ( size_t i = 0; i < m_Source.GetSensorCount(); ++i )
				m_Storage.Back()[ m_ChannelOfs + i ] = m_Source.GetSensorValue( i );
		}

		const StringIndexMap& ChannelSensorDelayAdapter::GetSensorNames() const 
		{
			return m_Source.GetSensorNames();
		}

	}
}
