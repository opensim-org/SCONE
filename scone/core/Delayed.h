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
			if ( !m_Data.empty() && current_time < m_Data.back().first )
				log::ErrorF( "Delayed<>::GetDelayed(): Wrong timestamp, current_time=%.6f < %.6f", current_time, m_Data.back().first );

			// add new item
			m_Data.push_back( std::make_pair( current_time, value ) );

			// remove all items from the front until front() + 1 < delayed time
			double delayed_time = current_time - m_Delay;
			while ( m_Data.size() > 2 && m_Data[ 1 ].first <= delayed_time )
				m_Data.pop_front();
		}

		double GetLastUpdateTime() { return m_Data.back().first; }
		double GetLastUpdateValue() { return m_Data.back().second; }

		T GetDelayed( double current_time )
		{
			SCONE_ASSERT( !m_Data.empty() );
			double delayed_time = current_time - m_Delay;

			// find first sample that is newer than delayed_time
			auto it1 = m_Data.begin();
			while ( it1->first <= delayed_time )
			{
				++it1;
				if ( it1 == m_Data.end() )
					return m_Data.back().second; // data is too old, give newest available sample
			}

			if ( it1 == m_Data.begin() )
				return m_Data.front().second; // data is too new, give oldest available sample

			// return interpolated value
			auto it0 = it1;
			--it0;

			// TODO: make this an ASSERT once we know it'll never happen
			if ( ! ( it0->first <= delayed_time && delayed_time < it1->first ) )
				log::ErrorF( "Delayed<>::GetDelayed(): Failed condition: t0=%.6f <= delayed_time=%.6f < t1=%.6f", it0->first, delayed_time, it1->first );

			double w = ( delayed_time - it0->first ) / ( it1->first - it0->first );
			return w * it1->second + ( 1 - w ) * it0->second;
		}
			
	private:
		double m_Delay;
		std::deque< std::pair< double, T > > m_Data;
	};

	typedef Delayed< Real > DelayedReal;
	typedef Delayed< double > DelayedDouble;
	typedef Delayed< float > DelayedFloat;
}
