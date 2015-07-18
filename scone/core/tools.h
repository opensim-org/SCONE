#pragma once

#include "core.h"
#include <iosfwd>
#include "PropNode.h"
#include "Log.h"

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

	/// Get file without extension (without dot)
	String CORE_API GetFileNameNoExt( const String& str );

	/// convert any streamable type to string
	template < typename T >
	String ToString( const T& value )
	{
		std::ostringstream str;
		str << value;
		return str.str();
	}

	/// Get clean class, removes everything before "::" (if present)
	template< typename T >
	String GetCleanClassName()
	{
		String str = typeid( T ).name();
		size_t pos = str.find_last_of(": ");
		if (pos != std::string::npos)
			return str.substr(pos + 1);
		else return str;
	}

	inline String GetQuoted( const String& s ) { return "\"" + s + "\""; }

	void CORE_API LogUntouched( const PropNode& p, log::Level level = log::WarningLevel, size_t depth = 0 );
}
