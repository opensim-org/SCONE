#include "HasSignature.h"
#include "propnode_tools.h"
#include "string_tools.h"
#include "xo/string/string_tools.h"
#include "system_tools.h"

namespace scone
{
	HasSignature::HasSignature( const PropNode& pn )
	{
		INIT_PROPERTY( pn, signature_prefix, String("") );
		INIT_PROPERTY( pn, signature_postfix, String("") );

		// replace DATE_TIME tag with (yes, indeed) DATE and TIME
		xo::replace_str( signature_prefix, "DATE_TIME", GetDateTimeAsString() );
		xo::replace_str( signature_postfix, "DATE_TIME", GetDateTimeAsString() );
		xo::replace_str( signature_prefix, "DATE_TIME_EXACT", GetDateTimeExactAsString() );
		xo::replace_str( signature_postfix, "DATE_TIME_EXACT", GetDateTimeExactAsString() );
		xo::replace_str( signature_prefix, "SCONE_BUILD", GetSconeBuildNumber() );
		xo::replace_str( signature_postfix, "SCONE_BUILD", GetSconeBuildNumber() );

		// add dots if they aren't there
		if ( !signature_postfix.empty() && signature_postfix.find_first_of( "._-" ) != 0 )
			signature_postfix = "." + signature_postfix;
		if ( !signature_prefix.empty() && signature_prefix.find_last_of( "._-" ) != signature_prefix.size() - 1 )
			signature_prefix = signature_prefix + ".";
	}

	HasSignature::~HasSignature()
	{
	}

	scone::String HasSignature::GetSignature() const
	{
		return signature_prefix + GetClassSignature() + signature_postfix;
	}
}
