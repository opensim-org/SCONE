#pragma once

#include "core.h"
#include <iosfwd>

namespace scone
{
	/// Get formatted string (printf style)
	String CORE_API GetStringF(const char* format, ...);

	/// Get formatted date/time string
	String CORE_API GetDateTimeAsString();

	/// Get clean name, removes "m_" (if present)
	String CORE_API GetCleanVarName( const String& str );

	/// Get file extension (without dot)
	String CORE_API GetFileNameExt( const String& str );

	/// convert any streamable type to string
	template < typename T >
	String ToString( const T& value )
	{
		std::ostringstream str;
		str << value;
		return str.str();
	}
}
