#include "PerturbationController.h"
#include "scone/sim/Model.h"

namespace scone
{
	namespace cs
	{
		PerturbationController::PerturbationController( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& target_area ) :
		Controller( props, par, model, target_area ),
		force_body( *FindByName( model.GetBodies(), props.get< String >( "body" ) ) ),
		current_force( 0 )
		{
			INIT_PROP( props, force, 100.0 );
			INIT_PROP( props, interval, 1.0 );
			INIT_PROP( props, duration, 0.1 );
			INIT_PROP( props, start, 0.0 );
		}

		void PerturbationController::StoreData( Storage<Real>::Frame& frame )
		{
		}

		sim::Controller::UpdateResult PerturbationController::UpdateControls( sim::Model& model, double timestamp )
		{
			if ( timestamp >= start && fmod( timestamp - start, interval ) < duration )
			{
				double dir = 2 * ( int( timestamp / interval ) % 2 ) - 1;
				current_force = Vec3( dir * force, 0, 0 );
				force_body.SetExternalForce( current_force );
			}

			return sim::Controller::SuccessfulUpdate;
		}

		String PerturbationController::GetClassSignature() const
		{
			return "P";
		}
	}
}
