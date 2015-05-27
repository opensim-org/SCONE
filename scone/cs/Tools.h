#pragma once

namespace scone
{
	namespace cs
	{
		inline Side GetSide( const String& str )
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
			if ( GetSide( str ) != NoSide )
				return str.substr( 0, str.length() - 2 );
			else return str;
		}

		inline String GetSideName( const Side& side )
		{
			if ( side == LeftSide ) return "_l";
			else if ( side == RightSide ) return "_r";
			else return "_m";
		}
	}

	template< typename T >
	T& FindNamedTrySided( std::vector< T >& cont, const String& name, const Side& side )
	{
		auto it = std::find_if( cont.begin(), cont.end(), [&]( T& item ) { return item->GetName() == name; } );
		if ( it == cont.end() ) // try sided name
			it = std::find_if( cont.begin(), cont.end(), [&]( T& item ) { return item->GetName() == name + GetSideName( side ); } );
		if ( it == cont.end() )
			SCONE_THROW( "Could not find " + GetQuoted( name ) + " or " + GetQuoted( name + GetSideName( side ) ) );

		return *it;
	}
}
