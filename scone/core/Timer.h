#pragma once

#include <memory>
#include "core.h"

//#include <boost/chrono/chrono.hpp>
#include <boost/timer/timer.hpp>

#define NSEC_IN_SEC 1000000000.0

namespace scone
{
	class CORE_API Timer
	{
	public:
		Timer();
		~Timer();
		void Restart();
		TimeInSeconds GetTime();
		TimeInSeconds Pause();
		TimeInSeconds Resume();

	private:
		TimeInSeconds GetCurrentTime();
		bool m_IsRunning;
		TimeInSeconds m_Time;

		// system specific implementation is hidden behind pimpl
		//class Impl;
		std::unique_ptr< boost::timer::cpu_timer > m_timer;
        //typedef boost::chrono::duration<double> boost_sec;
	};
}
