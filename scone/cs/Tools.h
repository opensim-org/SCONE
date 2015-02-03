#pragma once

namespace scone
{
	namespace cs
	{
		String GetSymmetricMuscleName( const String& str ) { return str.substr( 0, str.length() - 2 ); } 
		String ExtractNameAndSide( const String& str, String& name, Side& side ) {
			name = str.substr( 0, str.length() - 2 );
			side = ( str.back() == 'r' || str.back() == 'R' ) ? Right : Left;
			return name;
		}

		String GetSideName( const Side& side ) { if ( side == Left ) return "_L"; else if ( side == Right ) return "_R"; }
	}
}
