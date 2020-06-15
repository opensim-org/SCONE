#pragma once

#include "scone/core/types.h"
#include "Side.h"
#include "xo/string/string_cast.h"
#include "scone/core/string_tools.h"

namespace scone
{
	struct MuscleId
	{
		MuscleId( const String& str ) :
			side_( GetSideFromName( str ) ),
			base_( GetNameNoSide( str ) ),
			line_( no_index )
		{
			auto idx = str.find_last_of( "123456789" );
			if ( idx != String::npos && idx > 0 && base_[ idx - 1 ] != '_' )
			{
				xo::from_str( base_.substr( idx ), line_ );
				base_ = base_.substr( 0, idx );
			}
		}

		String base_line_name() const { return base_ + line_name(); }
		String base_line_side_name() const { return base_ + line_name() + side_name(); }
		String side_name() const { return GetSideName( side_ ); }
		String line_name() const { return line_ != no_index ? stringf( "%d", line_ ) : ""; }

		Side side_;
		String base_;
		index_t line_;
	};
}
