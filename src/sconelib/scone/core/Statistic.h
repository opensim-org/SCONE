/*
** Statistic.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "platform.h"
#include "types.h"
#include "Exception.h"

namespace scone
{
	template< typename T = Real >
	class Statistic
	{
	public:
		enum InterpolationMode { NoInterpolation, LinearInterpolation };

		Statistic( InterpolationMode im = LinearInterpolation ) : m_InterpolationMode( im ) { Reset(); }
		Statistic& operator=( const Statistic& other ) {
			m_Total = other.m_Total;
			m_Initial = other.m_Initial;
			m_Highest = other.m_Highest;
			m_Lowest = other.m_Lowest;
			m_PrevTime = other.m_PrevTime;
			m_PrevValue = other.m_PrevValue;
			m_InterpolationMode = other.m_InterpolationMode;
			m_nSamples = other.m_nSamples;
			return *this;
		}
		Statistic( const Statistic& other ) {
			*this = other;
		}

		void AddSample( TimeInSeconds timestamp, const T& value )
		{
			if ( m_nSamples == 0 )
			{
				m_StartTime = m_PrevTime = timestamp; // without interpolation, the first sample will be thrown away (since dt=0)
				m_PrevValue = m_Initial = m_Highest = m_Lowest = value;
			}

			SCONE_ASSERT( timestamp >= m_PrevTime );

			// update min / max
			m_Highest = std::max( m_Highest, value );
			m_Lowest = std::min( m_Lowest, value );

			// update average
			double dt = timestamp - m_PrevTime;
			switch( m_InterpolationMode )
			{
			case NoInterpolation: m_Total += dt * value; break;
			case LinearInterpolation: m_Total += dt * ( m_PrevValue + value ) / 2.0; break;
			}

			// update previous values for next call
			m_PrevTime = timestamp;
			m_PrevValue = value;
			m_nSamples++;
		}

		// add an 'unweighted' value
		void AddSample( const T& value )
		{
			// in 'unweighted mode', the number of samples always equals the end time.
			SCONE_ASSERT( m_PrevTime == m_nSamples && m_StartTime == 0 );

			if ( m_nSamples == 0 )
				m_PrevValue = m_Initial = m_Highest = m_Lowest = value;

			// update min / max
			m_Highest = std::max( m_Highest, value );
			m_Lowest = std::min( m_Lowest, value );

			// update average
			m_Total += value;
			m_nSamples++;
			m_PrevTime += 1.0;
			m_PrevValue = value;
		}

		void Reset()
		{
			m_PrevValue = m_Total = m_Highest = m_Lowest = T(0);
			m_PrevTime = m_StartTime = 0.0;
			m_nSamples = 0;
		}

		double GetPrevTime() { return m_PrevTime; }

		T GetAverage() const
		{
			if ( m_nSamples == 0 )
				return T(0);
			if ( m_nSamples == 1 || m_PrevTime == m_StartTime )
				return m_Initial; // only one sample was added or there was no dt
			else return m_Total / ( m_PrevTime - m_StartTime );
		}

		T GetHighest() const { return m_Highest; }
		T GetLowest() const { return m_Lowest; }
		T GetInitial() const { return m_Initial; }
		T GetLatest() const { return m_PrevValue; }
		T GetTotal() const { return m_Total; }
		size_t GetNumSamples() const { return m_nSamples; }

	private:
		T m_Total, m_Initial, m_Highest, m_Lowest, m_PrevValue;
		TimeInSeconds m_StartTime, m_PrevTime;
		InterpolationMode m_InterpolationMode;
		size_t m_nSamples;
	};
}
