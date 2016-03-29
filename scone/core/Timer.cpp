#include "stdafx.h"
#include "Timer.h"

/*
#ifdef _MSC_VER
#include <Windows.h>
#include <winbase.h>
#endif
*/
namespace scone
{
/*	class Timer::Impl
	{
	public:
		LARGE_INTEGER epoch;
		double pctoseconds;
	};
*/
	Timer::~Timer()
	{
	}

	Timer::Timer() :
/*	m_pImpl( new Impl ),
	m_Time( 0.0 )
	{
		LARGE_INTEGER freq;
		::QueryPerformanceFrequency( &freq );
		m_pImpl->pctoseconds = 1.0 / freq.QuadPart;

		Restart();
	}*/
    m_timer( new boost::timer::cpu_timer )
    {
        m_timer->start();
    }

	void Timer::Restart()
	{
//		m_IsRunning = true;
//		::QueryPerformanceCounter( &m_pImpl->epoch );
        m_timer->stop();
        m_timer->start();
	}

	TimeInSeconds Timer::GetTime()
	{
//		if ( m_IsRunning )
//			m_Time = GetCurrentTime();
        TimeInSeconds time = (m_timer->elapsed().wall) / NSEC_IN_SEC;
        //double time = sec;
		return time;
	}

	TimeInSeconds Timer::Pause()
	{
/*		if ( m_IsRunning )
		{
			m_Time = GetCurrentTime();
			m_IsRunning = false;
		}
*/
        m_timer->stop();
		return GetTime();
	}

	TimeInSeconds Timer::Resume()
	{
//		double deltaTime = GetCurrentTime() - m_Time;
//		m_pImpl->epoch.QuadPart += static_cast< LONGLONG >( deltaTime / m_pImpl->pctoseconds );
//		m_IsRunning = true;
        TimeInSeconds time = GetTime();
        m_timer->resume();
		return time;
	}

/*	TimeInSeconds Timer::GetCurrentTime()
	{
		LARGE_INTEGER current;
		::QueryPerformanceCounter( &current );
		return m_pImpl->pctoseconds * ( current.QuadPart - m_pImpl->epoch.QuadPart );


	}*/
}
