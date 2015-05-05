#include "stdafx.h"
#include "SensorDelayAdapter.h"

namespace scone
{
	namespace sim
	{
		SensorDelayAdapter::SensorDelayAdapter( Model& model, Sensor& source, double delay ) :
		m_Model( model ),
		m_Source( source ),
		m_PreviousUpdateStep( -1 )
		{
			// create delayers
			for ( size_t i = 0; i < source.GetSensorCount(); ++i )
				m_Delayers.push_back( Delayer< Real >( delay ) );
		}

		SensorDelayAdapter::~SensorDelayAdapter()
		{
		}

		size_t SensorDelayAdapter::GetSensorCount()
		{
			return m_Source.GetSensorCount();
		}

		const String& SensorDelayAdapter::GetSensorName( size_t idx )
		{
			return m_Source.GetSensorName( idx );
		}

		scone::Real SensorDelayAdapter::GetSensorValue( size_t idx )
		{
			UpdateFromSource(); // update when necessary
			return m_Delayers[ idx ].GetDelayedValue( m_Model.GetTime() );		
		}

		const String& SensorDelayAdapter::GetName() const 
		{
			return m_Source.GetName();
		}

		void SensorDelayAdapter::UpdateFromSource()
		{
			if ( m_Model.GetIntegrationStep() != m_PreviousUpdateStep )
			{
				// do some checks
				SCONE_CONDITIONAL_THROW( m_Model.GetIntegrationStep() != m_Model.GetPreviousIntegrationStep(), "SensorDelayAdapter should be updated at each new integration step" );
				SCONE_CONDITIONAL_THROW( m_Model.GetIntegrationStep() == m_PreviousUpdateStep + 1, "SensorDelayAdapter is missing data and should be updated at each new integration step" );

				for ( size_t i = 0; i < m_Source.GetSensorCount(); ++i )
					m_Delayers[ i ].AddSample( m_Model.GetTime(), m_Source.GetSensorValue( i ) );
			}
		}
	}
}
