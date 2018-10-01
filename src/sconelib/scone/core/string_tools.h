/*
** string_tools.h
**
** Copyright (C) 2013-2018 Thomas Geijtenbeek. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "platform.h"
#include "types.h"

#include "xo/string/string_tools.h"
#include "xo/string/string_cast.h"

namespace scone
{
	// import string tools from xo
	using xo::stringf;
	using xo::to_str;
	using xo::from_str;
	using xo::quoted;

	template< typename T > char GetSignChar( const T& v ) { return v < 0 ? '-' : '+'; }

	/// Get formatted date/time string
	String SCONE_API GetDateTimeAsString();

    /// Get formatted date/time with exact fractional seconds as string
    String SCONE_API GetDateTimeExactAsString();
}
