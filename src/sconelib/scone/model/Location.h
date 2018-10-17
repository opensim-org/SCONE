/*
** Location.h
**
** Copyright (C) 2013-2018 Thomas Geijtenbeek. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

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
		bool symmetric;

		String GetSidedName( const String& name ) const { return scone::GetSidedName( name, side ); }
		String GetParName( const String& name ) const { return symmetric ? GetNameNoSide( name ) : GetSidedName( name ); }
		Side GetSide() const { return side; }
		Location GetOpposite() const { return Location( GetOppositeSide( side ), symmetric ); }
	};

	// Find component by name & location
	template< typename T > T& FindByLocation( std::vector< T >& cont, const String& name, const Location& loc )
	{
		auto side = GetSideFromName( name );
		if ( side == NoSide ) {
			// name has no side, try object without side, then object with location side
			if ( auto it = TryFindByName( cont, name ); it != cont.end() )
				return *it;
			else return FindByName( cont, loc.GetSidedName( name ) );
		}
		else
		{
			// name has side, if location also has side, then mirror in case of LeftSide
			if ( loc.side == LeftSide )
				return FindByName( cont, GetSidedName( name, GetOppositeSide( side ) ) );
			else return FindByName( cont, name ); // use sided name
		}
	}
}
