#pragma once

#include <bitset>
#include "memory_tools.h"
#include "Side.h"

namespace scone
{
	class SCONE_API Area
	{
	public:
		Area( Side = NoSide, unsigned long long mask = 0 );
		~Area();

		std::bitset< LinkTypeCount > link_mask;
		Side side;

		static const Area WHOLE_BODY;
		static const Area LEFT_SIDE;
		static const Area RIGHT_SIDE;
		static const Area ROOT;
		static const Area LEFT_LEG;
		static const Area RIGHT_LEG;
	};

	Area MakeLegArea( Side side );
}
