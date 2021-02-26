/*
** HasSignature.cpp
**
** Copyright (C) 2013-2021 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "HasSignature.h"
#include "string_tools.h"
#include "xo/string/string_tools.h"
#include "system_tools.h"
#include "version.h"

namespace scone
{
	HasSignature::HasSignature( const PropNode& pn )
	{
		INIT_PROP( pn, signature_prefix, String("") );
		INIT_PROP( pn, signature_postfix, String("") );
		INIT_PROP( pn, signature, String("") );

		// replace DATE_TIME tag with (yes, indeed) DATE and TIME
		xo::replace_str( signature_prefix, "DATE_TIME_EXACT", GetDateTimeExactAsString() );
		xo::replace_str( signature_postfix, "DATE_TIME_EXACT", GetDateTimeExactAsString() );
		xo::replace_str( signature_prefix, "DATE_TIME", GetDateTimeAsString() );
		xo::replace_str( signature_postfix, "DATE_TIME", GetDateTimeAsString() );
		xo::replace_str( signature_prefix, "SCONE_BUILD", to_str( GetSconeVersion().build_ ) );
		xo::replace_str( signature_postfix, "SCONE_BUILD", to_str( GetSconeVersion().build_ ) );
		xo::replace_str( signature_prefix, "SCONE_VERSION", to_str( GetSconeVersion().build_ ) );
		xo::replace_str( signature_postfix, "SCONE_VERSION", to_str( GetSconeVersion().build_ ) );
	}

	String HasSignature::GetSignature() const
	{
		return xo::concatenate_str( { signature_prefix, signature.empty() ? GetClassSignature() : signature, signature_postfix }, "." );
	}
}
