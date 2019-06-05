/*
** Location.cpp
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
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
			return name_no_side + GetSideName( side_ );
		else if ( name_side == OppositeSide )
			return name_no_side + GetSideName( GetOppositeSide( side_ ) );
		else return name; // keep original side
	}
}
