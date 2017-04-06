#include "PerturbationController.h"
#include "scone/model/Model.h"
#include "scone/core/string_tools.h"

namespace scone
{
	PerturbationController::PerturbationController( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& target_area ) :
		Controller( props, par, model, target_area ),
		force_body( *FindByName( model.GetBodies(), props.get< String >( "body" ) ) ),
		current_force( 0 )
	{
		INIT_PROP( props, name, "" );
		INIT_PROP( props, force, 100.0 );
		INIT_PROP( props, interval, 1.0 );
		INIT_PROP( props, duration, 0.1 );
		INIT_PROP( props, start_time, 0.0 );
	}

	void PerturbationController::StoreData( Storage<Real>::Frame& frame )
	{
	}

	sim::Controller::UpdateResult PerturbationController::UpdateControls( sim::Model& model, double timestamp )
	{
		if ( timestamp >= start_time && fmod( timestamp - start_time, interval ) < duration )
		{
			double dir = 2 * ( int( timestamp / interval ) % 2 ) - 1;
			current_force = Vec3( dir * force, 0, 0 );
		}
		else current_force.clear();

		force_body.SetExternalForce( current_force );

		return sim::Controller::SuccessfulUpdate;
	}

	String PerturbationController::GetClassSignature() const
	{
		return stringf( "P%d", int( force ) );
	}
}
