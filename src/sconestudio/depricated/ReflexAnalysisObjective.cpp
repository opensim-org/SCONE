/*
** ReflexAnalysisObjective.cpp
**
** Copyright (C) 2013-2018 Thomas Geijtenbeek. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "ReflexAnalysisObjective.h"
#include "xo/utility/types.h"
#include "xo/string/string_tools.h"
#include <fstream>
#include "xo/container/prop_node_tools.h"
#include "scone/model/Side.h"
#include "xo/utility/irange.h"

using namespace xo;

namespace scone
{
	ReflexAnalysisObjective::ReflexAnalysisObjective( const Storage< Real >& data, const xo::prop_node& pn )
	{
		INIT_PROP( pn, use_velocity, false );
		INIT_PROP( pn, use_length, true );
		INIT_PROP( pn, use_force, true );
		INIT_PROP( pn, frame_rate, 1000 );
		INIT_PROP( pn, delay_multiplier, 2.0 );

		for ( Side s : { LeftSide, RightSide } )
		{
			for ( index_t idx = 0; idx < data.GetChannelCount(); ++idx )
			{
				auto label = data.GetLabels()[ idx ];
				auto mus_label = xo::left_of_str( label, "." );
				if ( GetSideFromName( mus_label ) == s )
				{
					if ( xo::str_ends_with( label, ".excitation" ) )
						excitations_.add_channel( mus_label, data.GetChannelData( idx ) );
					if ( use_length && xo::str_ends_with( label, ".fiber_length" ) )
						sensors_.add_channel( label, data.GetChannelData( idx ) );
					if ( use_force && xo::str_ends_with( label, ".fiber_force" ) )
						sensors_.add_channel( label, data.GetChannelData( idx ) );
					if ( use_velocity && xo::str_ends_with( label, ".fiber_velocity" ) )
						sensors_.add_channel( label, data.GetChannelData( idx ) );
				}
			}
		}

		// define parameters
		auto sym_mus = muscle_count() / 2;
		for ( index_t i = 0; i < sym_mus; ++i )
			info().add( spot::par_info( excitations_.get_label( i ), 0.05, 0.01 ) );
		for ( index_t si = 0; si < sensors_.channel_size(); ++si )
		{
			for ( index_t mi = 0; mi < sym_mus; ++mi )
				info().add( spot::par_info( excitations_.get_label( mi ) + "-" + sensors_.get_label( si ), 0.0, 0.01 ) );
		}

		muscle_delay.resize( muscle_count() );
		sensor_delay.resize( sensor_count() );
	}

	void ReflexAnalysisObjective::set_delays( const xo::prop_node& pn )
	{
		for ( auto& p : pn )
		{
			for ( index_t idx : xo::make_irange( excitations_.channel_size() ) )
			{
				if ( str_begins_with( excitations_.get_label( idx ), p.first ) )
					muscle_delay[ idx ] = p.second.get< double >();
			}
			for ( index_t idx : xo::make_irange( sensors_.channel_size() ) )
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
		for ( index_t fi = frame_rate; fi < frame_count; ++fi ) // skip first second
		{
			for ( index_t mi = 0; mi < sym_mus_count; ++mi )
			{
				double feedback = point[ mi ];
				int delay = static_cast< int >( frame_rate * delay_multiplier * muscle_delay[ mi ] );

				// feedback for left muscles
				for ( index_t si = 0; si < sensor_count(); ++si )
					feedback += point[ sym_mus_count + si * sym_mus_count + mi ] * sensors_( fi - delay, si );
				value += xo::squared( excitations_( fi, mi ) - feedback );

				// mirrored feedback for right muscles
				double mir_feedback = point[ mi ];
				for ( index_t si = 0; si < sensor_count(); ++si )
				{
					auto inv_si = ( si + sensor_count() / 2 ) % sensor_count();
					mir_feedback += point[ sym_mus_count + inv_si * sym_mus_count + mi ] * sensors_( fi - delay, si );
				}
				value += xo::squared( excitations_( fi, sym_mus_count + mi ) - mir_feedback );

			}
		}

		auto result = 100 * ( value / frame_count ) / muscle_count();
		return result;
	}

	void ReflexAnalysisObjective::save_report( const xo::path& filename, const spot::search_point& point )
	{
		std::ofstream str( filename.str() );
		str << "Signal";
		for ( xo::index_t i = 0; i < muscle_count() / 2; ++i )
			str << "\t" << excitations_.get_label( i );
		str << std::endl;

		str << "Constant";
		for ( xo::index_t i = 0; i < muscle_count() / 2; ++i )
			str << "\t" << point[ i ];
		str << std::endl;

		for ( xo::index_t si = 0; si < sensor_count(); ++si )
		{
			str << sensors_.get_label( si );
			for ( xo::index_t mi = 0; mi < muscle_count() / 2; ++mi )
				str << "\t" << point[ muscle_count() / 2 + si * muscle_count() / 2 + mi ];
			str << std::endl;
		}
	}
}
