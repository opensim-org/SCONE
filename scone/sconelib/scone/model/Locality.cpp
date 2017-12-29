#include "Locality.h"

namespace scone
{
	String Locality::ConvertName( const String& name ) const
	{
		auto org_side = ::scone::GetSideFromName( name );
		auto noside_name = GetNameNoSide( name );
		if ( org_side != NoSide )
		{
			if ( mirrored )
				return GetSidedName( noside_name, GetMirroredSide( org_side ) );
			else return name;
		}
		else // no side in initial name
		{
			return GetSidedName( noside_name, GetSide() );
		}
	}

	String Locality::GetName() const
	{
		return GetFullSideName( side ) + ( mirrored ? "Mirrored" : "" );
	}

	scone::Side Locality::GetSide() const
	{
		return mirrored ? GetMirroredSide( side ) : side;
	}
}
