#include "Area.h"

namespace scone
{
	const Area Area::WHOLE_BODY = Area( NoSide, ( 1 << LinkTypeCount ) - 1 );
	const Area Area::LEFT_SIDE = Area( LeftSide, ( 1 << LinkTypeCount ) - 1 );
	const Area Area::RIGHT_SIDE = Area( RightSide, ( 1 << LinkTypeCount ) - 1 );
	const Area Area::LEFT_LEG = MakeLegArea( LeftSide );
	const Area Area::RIGHT_LEG = MakeLegArea( RightSide );
	const Area Area::ROOT = Area( NoSide, RootLink );

	Area::Area( Side i_side, unsigned long long i_mask ) :
		side( i_side ),
		link_mask( i_mask )
	{
	}

	Area::~Area()
	{
	}

	scone::Area MakeLegArea( Side side )
	{
		Area sec( side, 0 );
		sec.link_mask.set( UpperLegLink );
		sec.link_mask.set( LowerLegLink );
		sec.link_mask.set( FootLink );

		return sec;
	}
}
