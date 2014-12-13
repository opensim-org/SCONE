#pragma once

#include <winbase.h>

namespace scone
{
	class Timer
	{
	public:
		Timer() {
			Reset();
		}

		void Reset() {
			LARGE_INTEGER freq;
			::QueryPerformanceFrequency( &freq );
			m_PerfCounterToTime = 1.0 / freq.QuadPart;
			::QueryPerformanceCounter( &m_Epoch );
		}

		double GetTime() {
			LARGE_INTEGER current;
			::QueryPerformanceCounter( &current );
			return m_PerfCounterToTime * ( current.QuadPart - m_Epoch.QuadPart );
		}

	private:
		LARGE_INTEGER m_Epoch;
		double m_PerfCounterToTime;
	};
}
