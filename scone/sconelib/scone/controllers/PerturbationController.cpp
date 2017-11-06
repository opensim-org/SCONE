#include "PerturbationController.h"
#include "scone/model/Model.h"
#include "scone/core/string_tools.h"
#include "flut/math/math.hpp"

namespace scone
{
	PerturbationController::PerturbationController( const PropNode& props, Params& par, Model& model, const Locality& target_area ) :
	Controller( props, par, model, target_area ),
	force_body( *FindByName( model.GetBodies(), props.get< String >( "body" ) ) ),
	current_force( 0 )
	{
		INIT_PROP( props, name, "" );
		INIT_PROP( props, force, 100.0 );
		INIT_PROP( props, interval, 2.0 );
		INIT_PROP( props, interval_min, interval );
		INIT_PROP( props, interval_max, interval );
		INIT_PROP( props, duration, 0.1 );
		INIT_PROP( props, start_time, 0.0 );
		INIT_PROP( props, end_time, 600.0 );
		INIT_PROP( props, position_offset, Vec3( 0, 0, 0 ) );
		position_offset = props.get< Vec3 >( "position_offset" );

		std::default_random_engine rng_engine;
		auto time_dist = std::uniform_real_distribution< TimeInSeconds >( interval_min, interval_max );
		auto dir_dist = std::uniform_real_distribution< double >( 0, 2 * flut::double_pi );
		perturbation_times.emplace_back( start_time, dir_dist( rng_engine ) );
		while ( perturbation_times.back().first < end_time )
			perturbation_times.emplace_back( perturbation_times.back().first + time_dist( rng_engine ), dir_dist( rng_engine ) );
	}

	Controller::UpdateResult PerturbationController::UpdateControls( Model& model, double timestamp )
	{
		// find closest perturbation time
		auto it = std::upper_bound( perturbation_times.begin(), perturbation_times.end(), std::make_pair( timestamp, 360.0 ) );
		if ( it != perturbation_times.begin() ) --it;
		auto ptime = it->first;
		auto pdir = it->second;
		if ( timestamp >= ptime && timestamp < ptime + duration )
			current_force = Vec3( cos( pdir ) * force, sin( pdir ) * force, 0 );
		else current_force.clear();

		force_body.SetExternalForceAtPoint( current_force, position_offset );

		return Controller::SuccessfulUpdate;
	}

	String PerturbationController::GetClassSignature() const
	{
		return stringf( "P%d", int( force ) );
	}
}
