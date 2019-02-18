/*
** ExternalBalanceController.cpp
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "ExternalBalanceController.h"

#include "scone/model/Model.h"
#include "scone/optimization/Params.h"

namespace scone
{
	ExternalBalanceController::ExternalBalanceController( const PropNode& props, Params& par, Model& model, const Location& target_area ) :
	Controller( props, par, model, target_area ),
	force_body( *FindByName( model.GetBodies(), props.get< String >( "body" ) ) )
	{
		INIT_PROP_REQUIRED( props, name );
		INIT_PAR( props, par, angular_velocity_gain, 0.0 );
		INIT_PAR( props, par, angular_velocity_target, 0.0 );
		INIT_PAR( props, par, angular_position_gain, 0.0 );
		INIT_PAR( props, par, angular_position_target, 0.0 );
	}

	void ExternalBalanceController::StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags ) const
	{
		double pitch_pos = xo::pitch( force_body.GetOrientation() ).rad_value();
		double pitch_vel = force_body.GetAngVel().z;
		frame[ name + ".pp" ] = pitch_pos;
		frame[ name + ".pv" ] = pitch_vel;
	}

	bool ExternalBalanceController::ComputeControls( Model& model, double timestamp )
	{
		return false;
	}

	String ExternalBalanceController::GetClassSignature() const
	{
		return "E";
	}
}
