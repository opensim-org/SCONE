/*
** SequentialController.cpp
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "SequentialController.h"
#include "xo/string/string_tools.h"
#include "xo/numerical/math.h"

namespace scone
{
	SequentialController::SequentialController( const PropNode& props, Params& par, Model& model, const Location& loc ) :
	CompositeController( props, par, model, loc )
	{
		SCONE_THROW_IF( controllers_.empty(), "No Controllers defined in SequentialController" );

		double time = 0.0;
		transition_times.push_back( time );
		if ( auto time_pn = props.try_get_child( "transition_intervals" ) )
		{
			for ( index_t idx = 0; idx < time_pn->size(); ++idx )
			{
				transition_intervals.push_back( par.get( xo::stringf( "dt%d", idx + 1 ), time_pn[ idx ] ) );
				transition_times.push_back( transition_times.back() + transition_intervals.back() );
			}
		}
	}

	bool SequentialController::ComputeControls( Model& model, double timestamp )
	{
		return controllers_[ GetActiveIdx( timestamp ) ]->UpdateControls( model, timestamp );
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
