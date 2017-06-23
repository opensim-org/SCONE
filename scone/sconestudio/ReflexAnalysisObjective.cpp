#include "ReflexAnalysisObjective.h"
#include "flut/system/types.hpp"
#include "flut/string_tools.hpp"

namespace scone
{
	ReflexAnalysisObjective::ReflexAnalysisObjective( const Storage< Real >& data )
	{
		for ( Index idx = 0; idx < data.GetChannelCount(); ++idx )
		{
			if ( flut::str_ends_with( data.GetLabels()[ idx ], ".excitation" ) )
				excitations_.add_channel( data.GetLabels()[ idx ], data.GetChannelData( idx ) );

			if ( flut::str_ends_with( data.GetLabels()[ idx ], ".fiber_length" )
				|| flut::str_ends_with( data.GetLabels()[ idx ], ".fiber_force" )
				|| flut::str_ends_with( data.GetLabels()[ idx ], ".fiber_velocity" ) )
				signals_.add_channel( data.GetLabels()[ idx ], data.GetChannelData( idx ) );
		}

		// define parameters
		for ( Index i = 0; i < excitations_.channel_size(); ++i )
			info().add( excitations_.get_label( i ), 0.0, 0.1 );

		for ( Index mi = 0; mi < excitations_.channel_size(); ++mi )
		{
			for ( Index si = 0; si < signals_.channel_size(); ++si )
				info().add( excitations_.get_label( mi ) + "-" + signals_.get_label( si ), 0.0, 0.01 );
		}
	}

	spot::fitness_t ReflexAnalysisObjective::evaluate( const spot::search_point& point ) const
	{
		double value = 0.0;
		size_t mus_count = excitations_.channel_size();
		size_t frame_count = excitations_.frame_size();
		for ( Index fi = 0; fi < frame_count; ++fi )
		{
			for ( Index mi = 0; mi < mus_count; ++mi )
			{
				double feedback = point[ mi ];
				for ( Index si = 0; si < signals_.channel_size(); ++si )
					feedback += point[ mus_count + mi * signals_.channel_size() + si ] * signals_( fi, si );

				value += flut::math::squared( excitations_( fi, mi ) - feedback );
			}
		}

		auto result = ( value / frame_count ) / mus_count;
		printf( "%3.0f ", 1000 * result );

		return result;
	}
}
