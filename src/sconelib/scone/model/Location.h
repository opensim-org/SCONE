#pragma once

#include "Side.h"
#include "scone/core/HasName.h"

namespace scone
{
	class SCONE_API Location
	{
	public:
		Location( Side s = NoSide, bool sym = false ) : side( s ), symmetric( s ) {}

		Side side;
		bool symmetric; // TODO: implement everywhere!

		String GetSidedName( const String& name ) const { return GetNameNoSide( name ) + GetSideName( side ); }
		String GetParName( const String& name ) const { return symmetric ? GetNameNoSide( name ) : GetSidedName( name ); }
		Side GetSide() const { return side; }
		Location GetOpposite() const { return Location( GetOppositeSide( side ), symmetric ); }
	};

	// Find component by name & location
	template< typename T > T& FindByLocation( std::vector< T >& cont, const String& name, const Location& loc )
	{
		if ( auto name_side = GetSideFromName( name ); name_side == NoSide ) {
			if ( auto it = TryFindByName( cont, name ); it != cont.end() )
				return *it; // object has name without side
			else return FindByName( cont, loc.GetSidedName( name ) ); // must have sided name
		}
		else return FindByName( cont, loc.GetSidedName( name ) );
	}
}
