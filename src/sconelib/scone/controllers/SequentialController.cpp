/*
** SequentialController.cpp
**
** Copyright (C) 2013-2021 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "SequentialController.h"
#include "xo/string/string_tools.h"
#include "xo/numerical/math.h"
#include "scone/core/string_tools.h"

namespace scone
{
	SequentialController::SequentialController( const PropNode& props, Params& par, Model& model, const Location& loc ) :
		CompositeController( props, par, model, loc ),
		active_idx_( 0 )
	{
		const PropNode& trans_pn = props.get_child( "transition_intervals" );
		SCONE_ERROR_IF( controllers_.empty(), "No Controllers defined in SequentialController" );
		SCONE_ERROR_IF( controllers_.size() - 1 != trans_pn.size(),
			"Wrong number of transition_intervals, expected " + to_str( controllers_.size() - 1 ) );

		double time = 0.0;
		transition_times.push_back( time );
		for ( index_t idx = 0; idx < trans_pn.size(); ++idx )
		{
			transition_intervals.push_back( par.get( xo::stringf( "transition%d", idx + 1 ), trans_pn[ idx ] ) );
			transition_times.push_back( transition_times.back() + transition_intervals.back() );
		}
	}

	bool SequentialController::ComputeControls( Model& model, double timestamp )
	{
		active_idx_ = GetActiveIdx( timestamp );
		return controllers_[ active_idx_ ]->UpdateControls( model, timestamp );
	}

	void SequentialController::StoreData( Storage<Real>::Frame& frame, const StoreDataFlags& flags ) const
	{
		auto name = GetName().empty() ? "SequentialController" : GetName();
		frame[ name + ".active_index" ] = static_cast<double>( active_idx_ );
		CompositeController::StoreData( frame, flags );
	}

	bool SequentialController::PerformAnalysis( const Model& model, double timestamp )
	{
		return controllers_[ GetActiveIdx( timestamp ) ]->UpdateAnalysis( model, timestamp );
	}

	xo::index_t SequentialController::GetActiveIdx( double timestamp )
	{
		auto it = std::upper_bound( transition_times.begin(), transition_times.end(), timestamp );
		return index_t( xo::clamped<int>( int( it - transition_times.begin() - 1 ), 0, int( controllers_.size() ) - 1 ) );
	}

	String SequentialController::GetClassSignature() const
	{
		String s = "C";
		for ( auto& c : controllers_ )
			s += '.' + c->GetSignature();
		return s;
	}
}
