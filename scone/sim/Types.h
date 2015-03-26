#pragma once

namespace scone
{
	/// Side enum, used for sidedness
	enum Side { LeftSide = -1, NoSide = 0, RightSide = 1 };
	inline Side GetMirroredSide( Side s ) { return static_cast< Side >( -s ); }
	enum LinkType { UnknownLink = 0, RootLink, UpperLegLink, LowerLegLink, FootLink, UpperArmLink, LowerArmLink, HandLink, SpineLink, HeadLink, LinkTypeCount };
}
