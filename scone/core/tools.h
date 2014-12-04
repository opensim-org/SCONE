#pragma once

#include "core.h"
#include <iosfwd>

namespace scone
{
	String CORE_API GetStringF(const char* format, ...);
	String CORE_API GetDateTimeAsString();

	template < typename T >
	String ToString( const T& value )
	{
		std::ostringstream str;
		str << value;
		return str.str();
	}
}
