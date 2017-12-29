#pragma once

#include "core.h"
#include <iosfwd>

#include "xo/string/string_tools.h"

// need for demangling with GCC
#ifndef _MSC_VER
#include <cxxabi.h>
#endif

namespace scone
{
	// import string tools from xo
	using xo::stringf;
	using xo::get_filename_ext;
	using xo::get_filename_without_ext;
	using xo::to_str;
	using xo::from_str;
	using xo::quoted;

	template< typename T > char GetSignChar( const T& v ) { return v < 0 ? '-' : '+'; }

	/// Get formatted date/time string
	String SCONE_API GetDateTimeAsString();

    /// Get formatted date/time with exact fractional seconds as string
    String SCONE_API GetDateTimeExactAsString();

	/// Get clean class, removes everything before "::" (if present)
	template< typename T >
	String GetCleanClassName()
	{
#ifdef _MSC_VER
		String str = typeid( T ).name();
#else
        const char* typeInfo = typeid ( T ).name();
        int status;
        char* cleanType = abi::__cxa_demangle(typeInfo, 0, 0, &status);
        String str = String(cleanType);
        free(cleanType);
#endif
		size_t pos = str.find_last_of(": ");
		if (pos != std::string::npos)
			return str.substr(pos + 1);
		else return str;
	}
}
