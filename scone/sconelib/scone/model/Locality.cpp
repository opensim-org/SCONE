#include "Locality.h"

namespace scone
{
	const String Locality::GetLocalName( const String& name ) const
	{
		auto org_side = GetSide( name );
		auto noside_name = GetNameNoSide( name );
		if ( org_side != NoSide )
		{
			if ( mirrored )
				return GetSidedName( noside_name, GetMirroredSide( org_side ) );
			else return name;
		}
		else return GetSidedName( noside_name, side );
	}
}
