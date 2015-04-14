#pragma once

namespace scone
{
	template< typename T >
	class Statistic
	{
	public:
		Statistic() { Reset(); }
			
		void AddSample( const T& value, const double& timestamp )
		{
			if ( m_PrevTime < 0.0 )
			{
				// first sample, initialize
				m_Initial = m_Highest = m_Lowest = value;
				m_Total = 0.0;
			}
			else
			{
				// update min / max
				m_Highest = std::max( m_Highest, value );
				m_Lowest = std::min( m_Lowest, value );

				// update average
				double dt = timestamp - m_PrevTime;
				m_Total += dt * ( m_PrevValue + value ) / 2.0;
			}

			// update previous values for next call
			m_PrevTime = timestamp;
			m_PrevValue = value;
		}

		void Reset()
		{
			m_Total = m_Highest = m_Lowest = 0.0;
			m_PrevTime = -1.0;
		}

		double GetPrevTime() { return m_PrevTime; }

		T GetAverage() const
		{
			if ( m_PrevTime < 0.0 )
				SCONE_THROW( "Cannont compute average, no samples added" );
			else if ( m_PrevTime == 0.0 )
				return m_Initial; // only one sample was added
			else return m_Total / m_PrevTime;
		}

		T GetHighest() const { return m_Highest; }
		T GetLowest() const { return m_Lowest; }
		T GetInitial() const { return m_Initial; }
		T GetLatest() const { return m_PrevValue; }
		T GetTotal() const { return m_Total; }

	private:
		T m_Total, m_Initial, m_Highest, m_Lowest, m_PrevTime, m_PrevValue;
	};

	typedef Statistic< Real > MeasuredReal;
}
