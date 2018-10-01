#include "PerturbationController.h"
#include "scone/model/Model.h"
#include "scone/core/string_tools.h"
#include "xo/numerical/math.h"
#include "scone/core/Log.h"

namespace scone
{
	PerturbationController::PerturbationController( const PropNode& props, Params& par, Model& model, const Location& target_area ) :
	Controller( props, par, model, target_area ),
	body( *FindByName( model.GetBodies(), props.get< String >( "body" ) ) ),
	current_force( 0 ),
	active_( false ),
	random_seed( props.get( "random_seed", 5489 ) ),
	rng_( random_seed )
	{
		INIT_PROP( props, force, Vec3::zero() );
		INIT_PROP( props, moment, Vec3::zero() );
		INIT_PROP( props, position_offset, Vec3::zero() );
		INIT_PROP( props, interval, 0 );
		INIT_PROP( props, duration, 0.1 );

		SCONE_THROW_IF( !interval.is_null() && duration.upper > interval.lower, "Duration cannot be longer than interval" );

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
		return stringf( "P%d", int( force.length() + moment.length() ) );
	}
}
