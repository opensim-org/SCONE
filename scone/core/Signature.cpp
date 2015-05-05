#include "stdafx.h"
#include "Signature.h"
#include "InitFromPropNode.h"

namespace scone
{
	Signature::Signature( const PropNode& pn )
	{
		INIT_PROPERTY( pn, signature_prefix, String("") );
		INIT_PROPERTY( pn, signature_postfix, String("") );

		// replace DATE_TIME tag with (yes, indeed) DATE and TIME
		if ( signature_prefix == "DATE_TIME" )
			signature_prefix = GetDateTimeAsString();
		if ( signature_postfix == "DATE_TIME" )
			signature_postfix = GetDateTimeAsString();

		// add dots if they aren't there
		if ( !signature_postfix.empty() && signature_postfix.find_first_of( "._-" ) != 0 )
			signature_postfix = "." + signature_postfix;
		if ( !signature_prefix.empty() && signature_prefix.find_last_of( "._-" ) != signature_prefix.size() - 1 )
			signature_prefix = signature_prefix + ".";
	}

	Signature::~Signature()
	{
	}

	scone::String Signature::GetSignature()
	{
		return signature_prefix + GetMainSignature() + signature_postfix;
	}
}
