#include "Locality.h"

namespace scone
{
	String Locality::ConvertName( const String& name ) const
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

	String Locality::GetName() const
	{
		return GetFullSideName( side ) + ( mirrored ? "Mirrored" : "" );
	}
}
