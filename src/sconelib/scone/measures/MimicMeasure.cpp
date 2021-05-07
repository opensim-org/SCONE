/*
** MimicMeasure.cpp
**
** Copyright (C) 2013-2021 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "MimicMeasure.h"

#include "scone/core/StorageIo.h"
#include "scone/model/Model.h"
#include "xo/numerical/math.h"
#include "scone/core/Log.h"
#include "scone/core/profiler_config.h"

namespace scone
{
	MimicMeasure::MimicMeasure( const PropNode& pn, Params& par, const Model& model, const Location& loc ) :
		Measure( pn, par, model, loc ),
		file( FindFile( pn.get<path>( "file" ) ) ),
		INIT_MEMBER( pn, include_states, xo::pattern_matcher( "*" ) ),
		INIT_MEMBER( pn, exclude_states, xo::pattern_matcher( "" ) ),
		INIT_MEMBER( pn, use_best_match, false ),
		INIT_MEMBER( pn, average_error_limit, 0 ),
		INIT_MEMBER( pn, peak_error_limit, 2 * average_error_limit ),
		INIT_MEMBER( pn, time_offset, 0 )
	{
		SCONE_PROFILE_FUNCTION( model.GetProfiler() );

		ReadStorageSto( storage_, file );
		SCONE_THROW_IF( storage_.IsEmpty(), file.str() + " contains no data" );

		// automatically set stop_time to match data, if not set
		if ( stop_time == 0 || stop_time > storage_.Back().GetTime() )
			stop_time = storage_.Back().GetTime();

		auto& s = model.GetState();
		for ( index_t state_idx = 0; state_idx < s.GetSize(); ++state_idx )
		{
			auto& name = s.GetName( state_idx );
			if ( include_states( name ) && !exclude_states( name ) )
			{
				index_t sto_idx = storage_.GetChannelIndex( name );
				if ( sto_idx != NoIndex )
				{
					state_storage_map_.emplace_back( state_idx, sto_idx );
					channel_errors_.emplace_back( name, 0.0 );
				}
			}
		}

		log::debug( "MimicMeasure found ", state_storage_map_.size(), " of ", storage_.GetChannelCount(), " channels from ", file );

		SCONE_THROW_IF( state_storage_map_.empty(), "No matching states found in " + file.str() );

		model.AddExternalResource( file );
	}

	bool MimicMeasure::UpdateMeasure( const Model& model, double timestamp )
	{
		// when using a full motion, skip when there's no more data
		// we don't terminate because there may be other measures
		if ( !use_best_match && timestamp > storage_.Back().GetTime() )
			return false;

		auto& s = model.GetState();
		double error = 0.0;
		index_t error_idx = 0;
		for ( auto& m : state_storage_map_ )
		{
			auto e = xo::squared( s[ m.first ] - storage_.GetInterpolatedValue( timestamp + time_offset, m.second ) );
			channel_errors_[ error_idx++ ].second = e;
			error += e;
		}

		error /= state_storage_map_.size();
		result_.AddSample( timestamp, error );

		if ( ( average_error_limit != 0 && result_.GetAverage() > average_error_limit ) ||
			( peak_error_limit != 0 && error > peak_error_limit ) )
			return true; // early termination

		return false;
	}

	double MimicMeasure::ComputeResult( const Model& model )
	{
		auto result = use_best_match ? result_.GetLowest() : result_.GetAverage();
		auto penalty_duration = xo::max( 0.0, xo::min( model.GetSimulationEndTime(), stop_time ) - model.GetTime() );
		auto penalty = peak_error_limit * penalty_duration;
		GetReport().set( "mimic_error", result );
		GetReport().set( "penalty", penalty );
		return result + penalty;
	}

	void MimicMeasure::StoreData( Storage<Real>::Frame& frame, const StoreDataFlags& flags ) const
	{
		frame[ "mimic_error" ] = result_.GetLatest();
		if ( flags.get<StoreDataTypes::DebugData>() )
		{
			for ( auto& c : channel_errors_ )
				frame[ c.first + "_mimic_error" ] = c.second;
		}
	}

	String MimicMeasure::GetClassSignature() const
	{
		return String( "M" );
	}
}
