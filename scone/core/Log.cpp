#include "stdafx.h"
#include "Log.h"

#include <boost/thread/mutex.hpp>

namespace scone
{
	boost::mutex g_LogMutex;

	void Log( const String& message )
	{
		boost::lock_guard< boost::mutex > lock( g_LogMutex );
		std::cout << message << std::endl;
	}
}
