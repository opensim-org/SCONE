/*
** Location.h
**
** Copyright (C) 2013-2021 Thomas Geijtenbeek and contributors. All rights reserved.
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
		Location( Side side = NoSide, bool symmetric = true ) : side_( side ), symmetric_( symmetric ) {}

		Side side_;
		bool symmetric_;

		String GetSidedName( const String& name ) const;
		String GetParName( const String& name ) const { return symmetric_ ? GetNameNoSide( name ) : GetSidedName( name ); }
		Side GetSide() const { return side_; }
		Location GetOpposite() const { return Location( GetOppositeSide( side_ ), symmetric_ ); }
	};

	// Find component by name & location
	template< typename T > const T& FindByLocation( const std::vector< T >& cont, const String& name, const Location& loc )
	{
		auto side = GetSideFromName( name );
		if ( side == NoSide ) {
			// name has no side, try object without side, then object with location side
			if ( auto it = TryFindByName( cont, name ); it != cont.end() )
				return *it;
		}

		// let location determine name
		return FindByName( cont, loc.GetSidedName( name ) );
	}
}
