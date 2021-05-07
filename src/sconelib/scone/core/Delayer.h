/*
** Delayer.h
**
** Copyright (C) 2013-2021 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include <deque>
#include "platform.h"
#include "types.h"

namespace scone
{
	// helper function that computes a weighted average of any value
	template< typename T >
	T GetWeightedAverage( double value1_weight, const T& value1, const T& value2 ) {
		return value1_weight * value1 + ( 1.0 - value1_weight ) * value2;
	}

	template< typename T >
	class Delayer
	{
	public:
		Delayer( double delay ) : m_Delay( delay ) { };
		virtual ~Delayer() { };

		void AddSample( double current_time, const T& value )
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

		T GetDelayedValue( double current_time )
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

			// #todo: make this an ASSERT once we know it'll never happen
			if ( ! ( it0->first <= delayed_time && delayed_time < it1->first ) )
				log::ErrorF( "Delayed<>::GetDelayed(): Failed condition: t0=%.6f <= delayed_time=%.6f < t1=%.6f", it0->first, delayed_time, it1->first );

			double w = ( delayed_time - it0->first ) / ( it1->first - it0->first );
			return GetWeightedAverage( w, it1->second, it0->second );
		}
			
	private:
		double m_Delay;
		std::deque< std::pair< double, T > > m_Data;
	};

	typedef Delayer< Real > DelayedReal;
	typedef Delayer< double > DelayedDouble;
	typedef Delayer< float > DelayedFloat;
}
