#pragma once

namespace scone
{
	namespace cs
	{
		String GetSymmetricMuscleName( const String& str )
		{
			return str.substr( 0, str.length() - 2 );
		} 

		String ExtractMuscleName( const String& str )
		{
			return str.substr( 0, str.length() - 2 );
		}

		Side ExtractMuscleSide( const String& str )
		{
			if ( str.length() >= 2 )
			{
				String substr = str.substr( str.size() - 2 );
				if ( substr == "_r" || substr == "_R" ) return RightSide;
				else if ( substr == "_l" || substr == "_L" ) return LeftSide;
			}

			return NoSide;
		}

		String GetSideName( const Side& side )
		{
			if ( side == LeftSide ) return "_L";
			else if ( side == RightSide ) return "_R";
			else return "_M";
		}
	}
}
