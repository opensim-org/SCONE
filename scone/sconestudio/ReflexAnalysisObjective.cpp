#include "ReflexAnalysisObjective.h"
#include "flut/system/types.hpp"
#include "flut/string_tools.hpp"
#include <fstream>
#include "flut/prop_node_tools.hpp"
#include "scone/model/Side.h"
#include "flut/math/irange.hpp"

using namespace flut;

namespace scone
{
	ReflexAnalysisObjective::ReflexAnalysisObjective( const Storage< Real >& data, const flut::prop_node& pn )
	{
		INIT_PROP( pn, use_velocity, false );
		INIT_PROP( pn, use_length, true );
		INIT_PROP( pn, use_force, true );
		INIT_PROP( pn, frame_rate, 1000 );
		INIT_PROP( pn, delay_multiplier, 2.0 );

		for ( Side s : { LeftSide, RightSide } )
		{
			for ( Index idx = 0; idx < data.GetChannelCount(); ++idx )
			{
				auto label = data.GetLabels()[ idx ];
				auto mus_label = flut::left_of_str( label, "." );
				if ( GetSideFromName( mus_label ) == s )
				{
					if ( flut::str_ends_with( label, ".excitation" ) )
						excitations_.add_channel( mus_label, data.GetChannelData( idx ) );
					if ( use_length && flut::str_ends_with( label, ".fiber_length" ) )
						sensors_.add_channel( label, data.GetChannelData( idx ) );
					if ( use_force && flut::str_ends_with( label, ".fiber_force" ) )
						sensors_.add_channel( label, data.GetChannelData( idx ) );
					if ( use_velocity && flut::str_ends_with( label, ".fiber_velocity" ) )
						sensors_.add_channel( label, data.GetChannelData( idx ) );
				}
			}
		}

		// define parameters
		auto sym_mus = muscle_count() / 2;
		for ( Index i = 0; i < sym_mus; ++i )
			info().add( excitations_.get_label( i ), 0.05, 0.01 );
		for ( Index si = 0; si < sensors_.channel_size(); ++si )
		{
			for ( Index mi = 0; mi < sym_mus; ++mi )
				info().add( excitations_.get_label( mi ) + "-" + sensors_.get_label( si ), 0.0, 0.01 );
		}

		muscle_delay.resize( muscle_count() );
		sensor_delay.resize( sensor_count() );
	}

	void ReflexAnalysisObjective::set_delays( const flut::prop_node& pn )
	{
		for ( auto& p : pn )
		{
			for ( Index idx : flut::make_irange( excitations_.channel_size() ) )
			{
				if ( str_begins_with( excitations_.get_label( idx ), p.first ) )
					muscle_delay[ idx ] = p.second.get< double >();
			}
			for ( Index idx : flut::make_irange( sensors_.channel_size() ) )
			{
				if ( str_begins_with( sensors_.get_label( idx ), p.first ) )
					sensor_delay[ idx ] = p.second.get< double >();
			}
		}
	}

	spot::fitness_t ReflexAnalysisObjective::evaluate( const spot::search_point& point ) const
	{
		double value = 0.0;
		auto sym_mus_count = muscle_count() / 2;
		size_t frame_count = excitations_.frame_size();
		for ( Index fi = frame_rate; fi < frame_count; ++fi ) // skip first second
		{
			for ( Index mi = 0; mi < sym_mus_count; ++mi )
			{
				double feedback = point[ mi ];
				int delay = static_cast< int >( frame_rate * delay_multiplier * muscle_delay[ mi ] );

				// feedback for left muscles
				for ( Index si = 0; si < sensor_count(); ++si )
					feedback += point[ sym_mus_count + si * sym_mus_count + mi ] * sensors_( fi - delay, si );
				value += flut::math::squared( excitations_( fi, mi ) - feedback );

				// mirrored feedback for right muscles
				double mir_feedback = point[ mi ];
				for ( Index si = 0; si < sensor_count(); ++si )
				{
					auto inv_si = ( si + sensor_count() / 2 ) % sensor_count();
					mir_feedback += point[ sym_mus_count + inv_si * sym_mus_count + mi ] * sensors_( fi - delay, si );
				}
				value += flut::math::squared( excitations_( fi, sym_mus_count + mi ) - mir_feedback );

			}
		}

		auto result = 100 * ( value / frame_count ) / muscle_count();
		return result;
	}

	void ReflexAnalysisObjective::save_report( const flut::path& filename, const spot::search_point& point )
	{
		std::ofstream str( filename.str() );
		str << "Signal";
		for ( flut::index_t i = 0; i < muscle_count() / 2; ++i )
			str << "\t" << excitations_.get_label( i );
		str << std::endl;

		str << "Constant";
		for ( flut::index_t i = 0; i < muscle_count() / 2; ++i )
			str << "\t" << point[ i ];
		str << std::endl;

		for ( flut::index_t si = 0; si < sensor_count(); ++si )
		{
			str << sensors_.get_label( si );
			for ( flut::index_t mi = 0; mi < muscle_count() / 2; ++mi )
				str << "\t" << point[ muscle_count() / 2 + si * muscle_count() / 2 + mi ];
			str << std::endl;
		}
	}
}
