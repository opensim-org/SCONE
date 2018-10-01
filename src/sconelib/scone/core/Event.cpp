/*
** Event.cpp
**
** Copyright (C) 2013-2018 Thomas Geijtenbeek. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "Event.h"

namespace scone
{
	std::vector< Event > DetectGrfEvents( const Storage< Real, TimeInSeconds>& s, double threshold, double timespan )
	{
		std::vector< Event > e;

		// find GRF channels
		index_t right_idx = s.GetChannelIndex( "leg1_r.grf_y" );

		// detect events
		for ( index_t frame_idx = 1; frame_idx < s.GetFrameCount(); ++frame_idx )
		{
			TimeInSeconds time = s.GetFrame( frame_idx ).GetTime();
			Real grf = s.GetFrame( frame_idx )[ right_idx ];
			Real prev_grf = s.GetFrame( frame_idx - 1 )[ right_idx ];

			if ( grf > 0 && prev_grf == 0 )
				e.push_back( Event{ time, Event::RightHeelStrike } );
			if ( grf == 0 && prev_grf > 0 )
				e.push_back( Event{ time, Event::RightToeOff } );
		}

		// filter events
		if ( e.size() > 2 )
		{
			for ( index_t i = 0; i < e.size() - 2; ++i )
			{
				if ( e[ i ].event == Event::RightHeelStrike )
				{
					// detect 'bump': short contact followed by long flight
					if ( e[ i + 1 ].time - e[ i ].time < timespan && e[ i + 2 ].time - e[ i + 1 ].time > timespan )
						e.erase( e.begin() + i, e.begin() + 2 );
				}
				else if ( e[ i ].event == Event::RightToeOff )
				{
					// detect short flight
					if ( e[ i + 1 ].time - e[ i ].time < timespan )
						e.erase( e.begin() + i, e.begin() + 2 );
				}
			}
		}

		return e;
	}

	SCONE_API Storage<> SliceData( const Storage<>& sto, const std::vector< Event >& events, Event e )
	{
		SCONE_THROW_NOT_IMPLEMENTED;
	}
}
