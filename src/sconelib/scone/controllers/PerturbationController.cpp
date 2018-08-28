#include "PerturbationController.h"
#include "scone/model/Model.h"
#include "scone/core/string_tools.h"
#include "xo/numerical/math.h"

namespace scone
{
	PerturbationController::PerturbationController( const PropNode& props, Params& par, Model& model, const Locality& target_area ) :
	Controller( props, par, model, target_area ),
	body_( *FindByName( model.GetBodies(), props.get< String >( "body" ) ) ),
	current_force( 0 ),
	active_( false )
	{
		INIT_PROP( props, name, "" );
		INIT_PROP( props, force, Vec3::zero() );
		INIT_PROP( props, moment, Vec3::zero() );
		INIT_PROP( props, position_offset, Vec3::zero() );
		INIT_PROP( props, interval, 2.0 );
		INIT_PROP( props, interval_min, interval );
		INIT_PROP( props, interval_max, interval );
		INIT_PROP( props, duration, 0.1 );
		INIT_PROP( props, start_time, 0.0 );
		INIT_PROP( props, end_time, 600.0 );
		INIT_PROP( props, random_seed, 5489 );

		std::default_random_engine rng_engine( random_seed );
		perturbation_times.emplace_back( start_time );
		if ( interval_min != 0.0 && interval_max != 0.0 )
		{
			auto time_dist = std::uniform_real_distribution< TimeInSeconds >( interval_min, interval_max );
			while ( perturbation_times.back() < end_time )
				perturbation_times.emplace_back( perturbation_times.back() + time_dist( rng_engine ) );
		}

		// set force point, make sure it's not set yet
		if ( !position_offset.is_null() )
		{
			if ( !body_.GetExternalForcePoint().is_null() && body_.GetExternalForcePoint() != position_offset )
				SCONE_THROW( "Cannot apply multiple external forces at different points on one body" );
			body_.SetExternalForceAtPoint( Vec3::zero(), position_offset );
		}
	}

	bool PerturbationController::ComputeControls( Model& model, double timestamp )
	{
		// find closest perturbation time
		auto it = std::upper_bound( perturbation_times.begin(), perturbation_times.end(), timestamp );
		if ( it != perturbation_times.begin() ) --it;
		bool active = ( timestamp >= *it && timestamp < *it + duration );

		if ( active != active_ )
		{
			double s = active ? 1 : -1;
			body_.AddExternalForce( s * force );
			body_.AddExternalMoment( s * moment );

			active_ = active;
		}

		return false;
	}

	String PerturbationController::GetClassSignature() const
	{
		return stringf( "P%d", int( force.length() + moment.length() ) );
	}
}
