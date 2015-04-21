#pragma once
#include <deque>

namespace scone
{
	// TODO: move to core
	template< typename T >
	class Delayed
	{
	public:
		Delayed( double delay ) : m_Delay( delay ) { };
		virtual ~Delayed() { };

		void Update( double current_time, const T& value )
		{
			// add new item
			m_Data.push_back( std::make_pair( current_time, value ) );

			// remove all items from the front until front() + 1 < delayed time
			double delayed_time = current_time - m_Delay;
			while ( m_Data.size() > 2 && m_Data[ 1 ].first <= delayed_time )
				m_Data.pop_front();
		}

		double GetLastUpdateTime() { return m_Data.back().first; }
		double GetLastUpdateValue() { return m_Data.back().second; }

		T GetDelayed()
		{
			SCONE_ASSERT( !m_Data.empty() );
			double delayed_time = GetLastUpdateTime() - m_Delay;

			if ( m_Data.size() == 1 || m_Data.front().first >= delayed_time )
				return m_Data.front().second; // not enough samples, return oldest

			// return interpolated value
			double t0 = m_Data.front().first;
			double t1 = m_Data[ 1 ].first;
			SCONE_ASSERT( delayed_time >= t0 && delayed_time < t1 )

			double w = ( delayed_time - t0 ) / ( t1 - t0 );
			return w * m_Data[ 1 ].second + ( 1 - w ) * m_Data.front().second;
		}
			
	private:
		double m_Delay;
		std::deque< std::pair< double, T > > m_Data;
	};

	typedef Delayed< Real > DelayedReal;
	typedef Delayed< double > DelayedDouble;
	typedef Delayed< float > DelayedFloat;
}
