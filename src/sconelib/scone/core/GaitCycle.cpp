#include "GaitCycle.h"
#include "Log.h"

namespace scone
{
	index_t FindNextTouch( const Storage<>& sto, index_t idx, const index_t channel_idx, const Real threshold ) {
		while ( idx != no_index && idx < sto.GetFrameCount() && sto.GetFrame( idx )[ channel_idx ] < threshold ) idx++;
		return idx < sto.GetFrameCount() ? idx : no_index;
	}

	index_t FindNextFlight( const Storage<>& sto, index_t idx, const index_t channel_idx, const Real threshold ) {
		while ( idx != no_index && idx < sto.GetFrameCount() && sto.GetFrame( idx )[ channel_idx ] >= threshold ) idx++;
		return idx < sto.GetFrameCount() ? idx : no_index;
	}

	std::vector<GaitCycle> ExtractGaitCycles( const Storage<>& sto, Real threshold, TimeInSeconds min_stance )
	{
		std::vector<GaitCycle> cycles;
		for ( auto side : { LeftSide, RightSide } )
		{
			string leg_name = ( side == LeftSide ) ? "leg0_l" : "leg1_r";
			index_t grf_chan = sto.GetChannelIndex( leg_name + ".grf_norm_y" );
			index_t cop_chan = sto.GetChannelIndex( leg_name + ".cop_x" );

			// skip to first touch down
			index_t flight_idx = FindNextFlight( sto, 0u, grf_chan, threshold );
			index_t touch_idx = FindNextTouch( sto, flight_idx, grf_chan, threshold );

			while ( touch_idx != no_index && touch_idx < sto.GetFrameCount() )
			{
				auto begin_time = sto.GetFrame( touch_idx ).GetTime();
				auto begin_pos = sto.GetFrame( touch_idx ).GetVec3( cop_chan );

				flight_idx = FindNextFlight( sto, touch_idx, grf_chan, threshold );
				if ( flight_idx == no_index )
					break;
				auto swing_time = sto.GetFrame( flight_idx ).GetTime();

				touch_idx = FindNextTouch( sto, flight_idx, grf_chan, threshold );
				if ( touch_idx == no_index )
					break;
				auto end_time = sto.GetFrame( touch_idx ).GetTime();
				auto end_pos = sto.GetFrame( touch_idx ).GetVec3( cop_chan );

				if ( swing_time - begin_time < min_stance )
				{
					// the stance was just a bump
					if ( !cycles.empty() && cycles.back().side_ == side )
					{
						// add it to the previous cycle
						cycles.back().end_ = end_time;
						cycles.back().end_pos_ = end_pos;
						log::trace( "U: ", cycles.back() );
					}
				}
				else
				{
					cycles.emplace_back( GaitCycle{ side, begin_time, swing_time, end_time, begin_pos, end_pos } );
					log::trace( "N: ", cycles.back() );
				}
			}
		}
		std::sort( cycles.begin(), cycles.end(), []( auto&& a, auto&& b ) { return a.begin_ < b.begin_; } );

		return cycles;
	}
}

xo::string xo::to_str( const scone::GaitCycle& c )
{
	auto str = GetFullSideName( c.side_ );
	str += xo::stringf( ": t_stance=%.3f t_swing=%.3f dur=%.3f len=%.3f", c.begin_, c.swing_, c.duration(), c.length() );
	str += " pb=" + to_str( c.begin_pos_ ) + " pe=" + to_str( c.end_pos_ );
	return str;
}
