/*
** Side.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "scone/core/platform.h"
#include "scone/core/memory_tools.h"
#include "scone/core/Exception.h"
#include <vector>

#include <algorithm>

#include "xo/string/string_tools.h"
#include "xo/string/string_cast.h"
#include "scone/core/types.h"
#include "scone/core/Vec3.h"

namespace scone
{
	enum Side {
		LeftSide = -1,
		NoSide = 0,
		RightSide = 1,
		OppositeSide = 999
	};

	inline Side GetOppositeSide( Side s ) {
		switch ( s )
		{
		case LeftSide: return RightSide;
		case NoSide: return NoSide;
		case RightSide: return LeftSide;
		default: SCONE_THROW( "Cannot determine opposite side" );
		}
	}

	inline Side GetSideFromName( const String& str )
	{
		if ( str.length() >= 2 )
		{
			String substr = xo::to_lower( str.substr( str.size() - 2 ) );
			if ( substr == "_r" ) return RightSide;
			else if ( substr == "_l" ) return LeftSide;
			else if ( substr == "_o" ) return OppositeSide;
		}

		return NoSide;
	}

	inline String GetNameNoSide( const String& str )
	{
		if ( GetSideFromName( str ) != NoSide )
			return str.substr( 0, str.length() - 2 );
		else return str;
	}

	inline String GetSideName( const Side& side )
	{
		if ( side == LeftSide ) return "_l";
		else if ( side == RightSide ) return "_r";
		else if ( side == OppositeSide ) return "_o";
		else return "";
	}

	inline String GetFullSideName( const Side& side )
	{
		if ( side == LeftSide ) return "Left";
		else if ( side == RightSide ) return "Right";
		else if ( side == OppositeSide ) return "Opposite";
		else return "NoSide";
	}

	inline String GetSidedName( const String& str, const Side& side )
	{
		return GetNameNoSide( str ) + GetSideName( side );
	}

	inline String GetMirroredName( const String& str )
	{
		return GetNameNoSide( str ) + GetSideName( GetOppositeSide( GetSideFromName( str ) ) );
	}

	// get direction vector that is mirrored in the XY plane for LeftSide, used in e.g. BodyOrientationSensor
	inline Vec3 GetSidedAxis( Vec3 axis, Side side ) {
		if ( side == LeftSide ) { axis.x = -axis.x; axis.y = -axis.y; }
		return axis;
	}

	// get direction vector that is mirrored in the XY plane for LeftSide, used in e.g. ComBosSensor
	inline Vec3 GetSidedDirection( Vec3 dir, Side side ) {
		if ( side == LeftSide ) dir.z = -dir.z;
		return dir;
	}

	template< typename T >
	T& FindBySide( std::vector< T >& cont, Side side )
	{
		using xo::to_str;
		auto it = std::find_if( cont.begin(), cont.end(), [&]( T& item ) { return item->GetSide() == side; } );
		SCONE_THROW_IF( it == cont.end(), "Could not find item with side " + to_str( side ) );
		return *it;
	}

	template< typename T >
	const T& FindByNameTrySided( const std::vector< T >& cont, const String& name, const Side& side )
	{
		using xo::quoted;
		auto it = std::find_if( cont.begin(), cont.end(), [&]( const T& item ) { return item->GetName() == name; } );
		if ( it == cont.end() ) // try sided name
			it = std::find_if( cont.begin(), cont.end(), [&]( const T& item ) { return item->GetName() == name + GetSideName( side ); } );
		if ( it == cont.end() )
			SCONE_THROW( "Could not find " + quoted( name ) + " or " + quoted( name + GetSideName( side ) ) );

		return *it;
	}
}

namespace xo
{
	inline bool from_str( const string& s, scone::Side& v )
	{
		if ( str_equals_any_of( s, { "left", "l" } ) )
			v = scone::LeftSide;
		else if ( str_equals_any_of( s, { "right", "r" } ) )
			v = scone::RightSide;
		else if ( str_equals_any_of( s, { "both", "none" } ) )
			v = scone::NoSide;
		else if ( str_equals_any_of( s, { "opposite", "other" } ) )
			v = scone::OppositeSide;
		else return false; // could not extract side
		return true;
	}
}
