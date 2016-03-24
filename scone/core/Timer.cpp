#include "Timer.h"

namespace scone
{
	class Timer::Impl
	{
	public:
		LARGE_INTEGER epoch;
		double pctoseconds;
	};

	Timer::~Timer()
	{
	}

	Timer::Timer() :
	m_pImpl( new Impl ),
	m_Time( 0.0 )
	{
		LARGE_INTEGER freq;
		::QueryPerformanceFrequency( &freq );
		m_pImpl->pctoseconds = 1.0 / freq.QuadPart;

		Restart();
	}

	void Timer::Restart()
	{
		m_IsRunning = true;
		::QueryPerformanceCounter( &m_pImpl->epoch );
	}

	TimeInSeconds Timer::GetTime()
	{
		if ( m_IsRunning )
			m_Time = GetCurrentTime();

		return m_Time;
	}

	TimeInSeconds Timer::Pause()
	{
		if ( m_IsRunning )
		{
			m_Time = GetCurrentTime();
			m_IsRunning = false;
		}

		return m_Time;
	}

	TimeInSeconds Timer::Resume()
	{
		double deltaTime = GetCurrentTime() - m_Time;
		m_pImpl->epoch.QuadPart += static_cast< LONGLONG >( deltaTime / m_pImpl->pctoseconds );
		m_IsRunning = true;
		return GetTime();
	}

	TimeInSeconds Timer::GetCurrentTime()
	{
		LARGE_INTEGER current;
		::QueryPerformanceCounter( &current );
		return m_pImpl->pctoseconds * ( current.QuadPart - m_pImpl->epoch.QuadPart );
	}
}
