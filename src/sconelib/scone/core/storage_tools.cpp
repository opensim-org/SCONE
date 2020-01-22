#include "storage_tools.h"
#include "xo/utility/frange.h"

namespace scone
{
	Storage<> ExtractNormalized( const Storage<>& sto, TimeInSeconds begin, TimeInSeconds end )
	{
		Storage<> new_sto( sto.GetLabels() );
		for ( Real perc : xo::frange<Real>( 0.0, 100.0, 0.5 ) )
		{
			auto inter = sto.GetInterpolatedFrame( begin + perc * ( end - begin ) / 100.0 );
			auto& f = new_sto.AddFrame( perc );
			for ( index_t i = 0; i < sto.GetChannelCount(); ++i )
				f[ i ] = inter.value( i );
		}
		return new_sto;
	}

	Storage<> ExtractGaitCycle( const Storage<>& sto, const String& force_channel, index_t cycle, const Real threshold )
	{
		SCONE_ASSERT( sto.GetFrameCount() > 0 );
		auto force_idx = sto.GetChannelIndex( force_channel );
		auto prev_force = sto.GetFrame( 0 )[ force_idx ];
		std::vector< TimeInSeconds > cycle_start_times;
		for ( index_t idx = 1; idx < sto.GetFrameCount(); ++idx )
		{
			auto force = sto.GetFrame( idx )[ force_idx ];
			if ( prev_force <= threshold && force > threshold )
				cycle_start_times.push_back( sto.GetFrame( idx ).GetTime() );
			prev_force = force;
		}

		if ( cycle == no_index ) // take middle cycle by default
			cycle = cycle_start_times.size() / 2;
		SCONE_ERROR_IF( cycle + 1 >= cycle_start_times.size(), "Invalid cycle" );

		return ExtractNormalized( sto, cycle_start_times[ cycle ], cycle_start_times[ cycle + 1 ] );
	}
}
