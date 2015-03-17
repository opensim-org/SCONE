#pragma once

namespace scone
{
	/// Side enum, used for sidedness
	enum Side { LeftSide = -1, NoSide = 0, RightSide = 1 };
	enum LinkType { UnknownLink = 0, RootLink, UpperLegLink, LowerLegLink, FootLink, UpperArmLink, LowerArmLink, HandLink, SpineLink, HeadLink, LinkTypeCount };
}
