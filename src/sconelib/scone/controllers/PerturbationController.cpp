/*
** PerturbationController.cpp
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "PerturbationController.h"
#include "scone/model/Model.h"
#include "scone/core/string_tools.h"
#include "xo/numerical/math.h"
#include "scone/core/Log.h"
#include <chrono>

using std::chrono::high_resolution_clock;

namespace scone
{
	PerturbationController::PerturbationController( const PropNode& props, Params& par, Model& model, const Location& target_area ) :
		Controller( props, par, model, target_area ),
		body( *FindByName( model.GetBodies(), props.get< String >( "body" ) ) ),
		active_( false ),
		current_force(),
		current_moment()
	{
		INIT_PROP( props, force, Vec3::zero() );
		INIT_PROP( props, moment, Vec3::zero() );
		INIT_PROP( props, position_offset, Vec3::zero() );
		INIT_PROP( props, interval, xo::bounds<TimeInSeconds>( 0, 0 ) );
		INIT_PROP( props, duration, xo::bounds<TimeInSeconds>( 0.1, 0.1 ) );
		INIT_PROP( props, random_seed,
				   high_resolution_clock::now().time_since_epoch().count() );
		rng_ = xo::random_number_generator( random_seed );

		SCONE_THROW_IF( !interval.is_null() && duration.upper > interval.lower, "Duration cannot be longer than interval" );
		SCONE_ERROR_IF( !model.GetFeatures().allow_external_forces, "External forces are not enabled for this model, please add:\n\nuse_external_forces = 1" );

		AddPerturbation();

		// set force point, make sure it's not set yet
		if ( !position_offset.is_null() )
		{
			if ( !body.GetExternalForcePoint().is_null() && body.GetExternalForcePoint() != position_offset )
				SCONE_THROW( "Cannot apply multiple external forces at different points on one body" );
			body.SetExternalForceAtPoint( Vec3::zero(), position_offset );
		}
	}

	void PerturbationController::AddPerturbation()
	{
		Perturbation p;
		p.start = perturbations.empty() ? start_time : perturbations.back().start + rng_.uni( interval );
		p.stop = p.start + rng_.uni( duration );
		p.force = force;
		p.moment = moment;
		perturbations.emplace_back( p );
	}

	bool PerturbationController::ComputeControls( Model& model, double timestamp )
	{
		if ( !interval.is_null() && perturbations.back().start < timestamp && ( stop_time == 0.0 || timestamp < stop_time ) )
			AddPerturbation();

		bool active = false;
		auto it = std::upper_bound( perturbations.begin(), perturbations.end(), timestamp, [&]( const TimeInSeconds& t, const Perturbation& p ) { return t < p.start; } );
		if ( it != perturbations.begin() )
			active = ( --it )->is_active( timestamp );

		if ( active != active_ )
		{
			log::trace( timestamp, ": Changing perturbation state to ", active );
			body.AddExternalForce( active ? force : -force );
			body.AddExternalMoment( active ? moment : -moment );
			active_ = active;
		}

		return false;
	}

	String PerturbationController::GetClassSignature() const
	{
		return stringf( "P%d", int( xo::length( force ) + xo::length( moment ) ) );
	}
}
