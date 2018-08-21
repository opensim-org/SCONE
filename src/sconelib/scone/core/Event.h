#pragma once
#include "platform.h"
#include <vector>
#include "Storage.h"
#include "types.h"

namespace scone
{
	struct Event
	{
		enum Type
		{
			NoEvent,
			LeftHeelStrike,
			RightHeelStrike,
			LeftToeOff,
			RightToeOff
		};

		TimeInSeconds time;
		Type event;
	};

	SCONE_API std::vector< Event > DetectGrfEvents( const Storage< Real, TimeInSeconds>& s, double threshold = 0.0001, double duration_filter = 0.2 );
	SCONE_API Storage<> SliceData( const Storage<>& sto, const std::vector< Event >& events, Event e );
}
