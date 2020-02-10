#include "GaitCycle.h"

namespace scone
{
	index_t FindNextAbove( const Storage<>& sto, index_t idx, const index_t channel_idx, const Real threshold ) {
		while ( idx != no_index && idx < sto.GetFrameCount() && sto.GetFrame( idx )[ channel_idx ] < threshold ) idx++;
		return idx < sto.GetFrameCount() ? idx : no_index;
	}

	index_t FindNextBelow( const Storage<>& sto, index_t idx, const index_t channel_idx, const Real threshold ) {
		while ( idx != no_index && idx < sto.GetFrameCount() && sto.GetFrame( idx )[ channel_idx ] >= threshold ) idx++;
		return idx < sto.GetFrameCount() ? idx : no_index;
	}

	std::vector<GaitCycle> ExtractGaitCycles( const Storage<>& sto, Real threshold, TimeInSeconds min_stance )
	{
		std::vector<GaitCycle> cycles;
		for ( auto side : { LeftSide, RightSide } )
		{
			string leg_name = ( side == LeftSide ) ? "leg0_l": "leg1_r";
			index_t grf_idx = sto.GetChannelIndex( leg_name + ".grf_norm_y" );
			index_t cop_idx = sto.GetChannelIndex( leg_name + ".cop_x" );

			// skip to first touch down
			index_t idx = FindNextBelow( sto, 0u, grf_idx, threshold );
			idx = FindNextAbove( sto, idx, grf_idx, threshold );

			while ( idx != no_index && idx < sto.GetFrameCount() )
			{
				auto begin_time = sto.GetFrame( idx ).GetTime();
				auto begin_pos = sto.GetFrame( idx ).GetVec3( cop_idx );
				idx = FindNextBelow( sto, idx, grf_idx, threshold );
				if ( idx == no_index ) break;
				auto swing_time = sto.GetFrame( idx ).GetTime();
				idx = FindNextAbove( sto, idx, grf_idx, threshold );
				if ( idx == no_index ) break;
				auto end_time = sto.GetFrame( idx ).GetTime();
				auto end_pos = sto.GetFrame( idx ).GetVec3( cop_idx );

				if ( end_time - begin_time < min_stance )
				{
					// this cycle was actually just a bump
					if ( !cycles.empty() )
					{
						cycles.back().end_ = end_time;
						cycles.back().end_pos_ = end_pos;
					}
				}
				else cycles.emplace_back( GaitCycle{ side, begin_time, swing_time, end_time, begin_pos, end_pos } );
			}
		}
		std::sort( cycles.begin(), cycles.end(), []( auto&& a, auto&& b ) { return a.begin_ < b.begin_; } );

		return cycles;
	}
}

xo::string xo::to_str( const scone::GaitCycle& c )
{
	return GetFullSideName( c.side_ ) + xo::stringf( ": begin=%.3f swing=%.3f duration=%.3f length=%.3f", c.begin_, c.swing_, c.duration(), c.length() );
}
