#pragma once

#include "core.h"
#include <iosfwd>

#include "flut/string_tools.hpp"
#include "String.h"

// need for demangling with GCC
#ifndef _MSC_VER
#include <cxxabi.h>
#endif

namespace scone
{
	// import string tools from flut
	using flut::stringf;
	using flut::get_filename_ext;
	using flut::get_filename_without_ext;
	using flut::to_str;
	using flut::from_str;
	using flut::quoted;

	/// Get formatted date/time string
	String SCONE_API GetDateTimeAsString();

    /// Get formatted date/time with exact fractional seconds as string
    String SCONE_API GetDateTimeExactAsString();

	/// Get clean name, removes "m_" (if present)
	String SCONE_API GetCleanVarName( const String& str );

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
