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
		double GetTime();
		double Pause();
		double Resume();

	private:
		double GetCurrentTime();
		bool m_IsRunning;
		double m_Time;

		// system specific implementation is hidden behind pimpl
		class Impl;
		std::unique_ptr< Impl > m_pImpl;
	};
}
