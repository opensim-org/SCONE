/*
** Location.cpp
**
** Copyright (C) 2013-2018 Thomas Geijtenbeek. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "Location.h"
#include "Side.h"

namespace scone
{
	String Location::GetSidedName( const String& name ) const
	{
		auto name_side = GetSideFromName( name );
		auto name_no_side = GetNameNoSide( name );
	
		if ( name_side == NoSide ) // no side in name, add location side
			return name_no_side + GetSideName( side );
		else if ( side == LeftSide ) // side in name, mirror
			return name_no_side + GetSideName( GetOppositeSide( name_side ) );
		else return name; // keep original side
	}
}
