
#include "HasSignature.h"
#include "InitFromPropNode.h"

namespace scone
{
	HasSignature::HasSignature( const PropNode& pn )
	{
		INIT_PROPERTY( pn, signature_prefix, String("") );
		INIT_PROPERTY( pn, signature_postfix, String("") );

		// replace DATE_TIME tag with (yes, indeed) DATE and TIME
		if ( signature_prefix == "DATE_TIME" )
			signature_prefix = GetDateTimeAsString();
		if ( signature_postfix == "DATE_TIME" )
			signature_postfix = GetDateTimeAsString();

        // use DATE_TIME_EXACT to add fractional seconds. meant for use when
        // there may be folder conflicts (i.e. running many in parallel on 
        // a cluster)
        if ( signature_prefix == "DATE_TIME_EXACT" )
            signature_prefix = GetDateTimeExactAsString();
        if ( signature_postfix == "DATE_TIME_EXACT" )
            signature_postfix = GetDateTimeExactAsString();

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
