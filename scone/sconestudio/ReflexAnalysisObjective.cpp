#include "ReflexAnalysisObjective.h"
#include "flut/system/types.hpp"
#include "flut/string_tools.hpp"
#include <fstream>

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
				sensors_.add_channel( data.GetLabels()[ idx ], data.GetChannelData( idx ) );
		}

		// define parameters
		for ( Index i = 0; i < excitations_.channel_size(); ++i )
			info().add( excitations_.get_label( i ), 0.1, 0.1 );

		for ( Index si = 0; si < sensors_.channel_size(); ++si )
		{
			for ( Index mi = 0; mi < excitations_.channel_size(); ++mi )
				info().add( excitations_.get_label( mi ) + "-" + sensors_.get_label( si ), 0.0, 0.01 );
		}
	}

	spot::fitness_t ReflexAnalysisObjective::evaluate( const spot::search_point& point ) const
	{
		double value = 0.0;
		size_t frame_count = excitations_.frame_size();
		for ( Index fi = 0; fi < frame_count; ++fi )
		{
			for ( Index mi = 0; mi < muscle_count(); ++mi )
			{
				double feedback = point[ mi ];
				for ( Index si = 0; si < sensor_count(); ++si )
					feedback += point[ muscle_count() + si * muscle_count() + mi ] * sensors_( fi, si );

				value += flut::math::squared( excitations_( fi, mi ) - feedback );
			}
		}

		auto result = 100 * ( value / frame_count ) / muscle_count();
		return result;
	}

	void ReflexAnalysisObjective::save_report( const flut::path& filename, const spot::search_point& point )
	{
		std::ofstream str( filename.str() );
		str << "Signal";
		for ( auto& l : excitations_.labels() )
			str << "\t" << l;
		str << std::endl;

		str << "Constant";
		for ( flut::index_t i = 0; i < muscle_count(); ++i )
			str << "\t" << point[ i ];
		str << std::endl;

		for ( flut::index_t si = 0; si < sensor_count(); ++si )
		{
			str << sensors_.get_label( si );
			for ( flut::index_t mi = 0; mi < muscle_count(); ++mi )
				str << "\t" << point[ muscle_count() + si * muscle_count() + mi ];
			str << std::endl;
		}
	}
}
