#pragma once

namespace scone
{
	template< typename T >
	class MeasuredValue
	{
	public:
		MeasuredValue() { Reset(); }
			
		void AddSample( const T& value, const double& timestamp )
		{
			if ( m_PrevTime < 0.0 )
			{
				// first sample, initialize
				m_Initial = m_Highest = m_Lowest = value;
				m_Total = m_Weight = 0.0;
			}
			else
			{
				// update min / max
				m_Highest = std::max( m_Highest, value );
				m_Lowest = std::min( m_Lowest, value );

				// update average
				double dt = timestamp - m_PrevTime;
				m_Total += dt * ( m_PrevValue + value ) / 2.0;
				m_Weight += dt;
			}

			// update previous values for next call
			m_PrevTime = timestamp;
			m_PrevValue = value;
		}

		void Reset()
		{
			m_Total = m_Weight = m_Highest = m_Lowest = 0.0;
			m_PrevTime = -1.0;
		}

		T GetAverage() const
		{
			if ( m_PrevTime < 0.0 )
				SCONE_THROW( "Cannont compute average, no samples added" );
			else if ( m_Weight == 0.0 )
				return m_Initial; // only one sample was added
			else return m_Total / m_Weight;
		}

		T GetHighest() const { return m_Highest; }
		T GetLowest() const { return m_Lowest; }
		T GetInitial() const { return m_Initial; }
		T GetLatest() const { return m_PrevValue; }

	private:
		T m_Total, m_Weight, m_Initial, m_Highest, m_Lowest, m_PrevTime, m_PrevValue;
	};

	typedef MeasuredValue< Real > MeasuredReal;
}
