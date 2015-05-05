#pragma once

#include <memory>
#include "core.h"

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
		class Impl;
		std::unique_ptr< Impl > m_pImpl;
	};
}
