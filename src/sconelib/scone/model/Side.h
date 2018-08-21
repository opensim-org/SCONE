#pragma once

#include "scone/core/core.h"
#include "scone/core/memory_tools.h"
#include "scone/core/Exception.h"
#include <vector>

#include <algorithm>

#include "xo/string/string_tools.h"
#include "scone/core/types.h"

namespace scone
{
	enum Side {
		LeftSide = -1,
		NoSide = 0,
		RightSide = 1
	};

	inline Side GetMirroredSide( Side s ) { return static_cast< Side >( -s ); }

	inline Side GetSideFromName( const String& str )
	{
		if ( str.length() >= 2 )
		{
			String substr = str.substr( str.size() - 2 );
			if ( substr == "_r" || substr == "_R" ) return RightSide;
			else if ( substr == "_l" || substr == "_L" ) return LeftSide;
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
		else return "";
	}

	inline String GetFullSideName( const Side& side )
	{
		if ( side == LeftSide ) return "Left";
		else if ( side == RightSide ) return "Right";
		else return "NoSide";
	}

	inline String GetSidedName( const String& str, const Side& side )
	{
		return GetNameNoSide( str ) + GetSideName( side );
	}

	inline String GetMirroredName( const String& str )
	{
		return GetNameNoSide( str ) + GetSideName( GetMirroredSide( GetSideFromName( str ) ) );
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
	T& FindByNameTrySided( std::vector< T >& cont, const String& name, const Side& side )
	{
		using xo::quoted;
		auto it = std::find_if( cont.begin(), cont.end(), [&]( T& item ) { return item->GetName() == name; } );
		if ( it == cont.end() ) // try sided name
			it = std::find_if( cont.begin(), cont.end(), [&]( T& item ) { return item->GetName() == name + GetSideName( side ); } );
		if ( it == cont.end() )
			SCONE_THROW( "Could not find " + quoted( name ) + " or " + quoted( name + GetSideName( side ) ) );

		return *it;
	}
}
