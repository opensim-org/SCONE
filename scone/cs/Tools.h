#pragma once

namespace scone
{
	namespace cs
	{
		String GetSymmetricMuscleName( const String& str ) { return str.substr( 0, str.length() - 2 ); } 
	}
}
