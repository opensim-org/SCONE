#include "ExternalBalanceController.h"

#include "scone/model/Model.h"
#include "scone/optimization/Params.h"

namespace scone
{

	ExternalBalanceController::ExternalBalanceController( const PropNode& props, Params& par, Model& model, const Locality& target_area ) :
	Controller( props, par, model, target_area ),
	force_body( *FindByName( model.GetBodies(), props.get< String >( "body" ) ) )
	{
		INIT_PROP_REQUIRED( props, name );
		INIT_PARAM( props, par, angular_velocity_gain, 0.0 );
		INIT_PARAM( props, par, angular_velocity_target, 0.0 );
		INIT_PARAM( props, par, angular_position_gain, 0.0 );
		INIT_PARAM( props, par, angular_position_target, 0.0 );
	}

	void ExternalBalanceController::StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags ) const
	{
		double pitch_pos = flut::math::pitch( force_body.GetOrientation() ).rad_value();
		double pitch_vel = force_body.GetAngVel().z;
		frame[ name + ".pp" ] = pitch_pos;
		frame[ name + ".pv" ] = pitch_vel;
	}

	scone::Controller::UpdateResult ExternalBalanceController::UpdateControls( Model& model, double timestamp )
	{
		return Controller::SuccessfulUpdate;
	}

	String ExternalBalanceController::GetClassSignature() const
	{
		return "E";
	}
}
