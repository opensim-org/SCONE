#pragma once

namespace scone
{
	namespace cs
	{
		String GetSymmetricMuscleName( const String& str ) { return str.substr( 0, str.length() - 2 ); } 
		String ExtractNameAndSide( const String& str, String& name, Side& side ) {
			name = str.substr( 0, str.length() - 2 );
			side = ( str.back() == 'r' || str.back() == 'R' ) ? RightSide : LeftSide;
			return name;
		}

		String GetSideName( const Side& side ) { if ( side == LeftSide ) return "_L"; else if ( side == RightSide ) return "_R"; }
	}
}
