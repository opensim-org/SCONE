#pragma once

#include "core.h"
#include <iosfwd>
#include "PropNode.h"
#include "Log.h"

#include "flut/string_tools.hpp"

namespace scone
{
	// import string tools from flut
	using flut::stringf;
	using flut::get_filename_ext;
	using flut::get_filename_without_ext;
	using flut::glob_match;
	using flut::make_str;
	using flut::from_string;
	using flut::quoted;

	/// Get formatted date/time string
	String CORE_API GetDateTimeAsString();

	/// Get clean name, removes "m_" (if present)
	String CORE_API GetCleanVarName( const String& str );

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

	void CORE_API LogUntouched( const PropNode& p, log::Level level = log::WarningLevel, size_t depth = 0 );
}
