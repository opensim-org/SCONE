#pragma once

namespace scone
{
	namespace cs
	{
		inline String GetNameNoSide( const String& str )
		{
			return str.substr( 0, str.length() - 2 );
		}

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

		inline String GetSideName( const Side& side )
		{
			if ( side == LeftSide ) return "_l";
			else if ( side == RightSide ) return "_r";
			else return "_m";
		}
	}
}
