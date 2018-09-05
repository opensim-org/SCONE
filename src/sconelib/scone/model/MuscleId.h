#pragma once

#include "scone/core/types.h"
#include "Side.h"
#include "xo/string/string_cast.h"

namespace scone
{
	struct MuscleId
	{
		MuscleId( const String& str ) : side_( GetSideFromName( str ) ), base_( GetNameNoSide( str ) ), line_( 0 ) {
			auto idx = str.find_first_of( "0123456789" );
			if ( idx != String::npos )
			{
				line_ = xo::from_str< size_t >( base_.substr( idx ), 0 );
				base_ = base_.substr( 0, idx );
			}
		}

		String side_name() const { return GetSideName( side_ ); }

		String base_;
		index_t line_;
		Side side_;
	};
}
