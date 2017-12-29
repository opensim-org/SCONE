#include "Actuator.h"

namespace scone
{
	Actuator::Actuator() : m_ActuatorControlValue( 0.0 ), m_DelaySamples( 0 )
	{
	}

	Actuator::~Actuator()
	{
	}

	void Actuator::ClearInput()
	{
		if ( m_DelaySamples > 0 )
		{
			m_DelayBuffer.push_back( m_ActuatorControlValue );
			if ( m_DelayBuffer.size() > m_DelaySamples )
				m_DelayBuffer.pop_front();
		}
		m_ActuatorControlValue = 0.0;
	}

	double Actuator::GetInput() const
	{
		if ( !m_DelayBuffer.empty() )
			return m_DelayBuffer.front();
		else return m_ActuatorControlValue;
	}

	void Actuator::StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags ) const
	{
	}

	void Actuator::SetDelay( TimeInSeconds d, TimeInSeconds cs )
	{
		m_DelaySamples = std::lround( d / cs );
	}

	TimeInSeconds Actuator::GetDelay( TimeInSeconds cs )
	{
		return m_DelaySamples * cs;
	}
}
