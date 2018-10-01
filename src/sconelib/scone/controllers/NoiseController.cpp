/*
** NoiseController.cpp
**
** Copyright (C) 2013-2018 Thomas Geijtenbeek. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "NoiseController.h"
#include "scone/model/Model.h"
#include "scone/model/Actuator.h"

namespace scone
{
	
	NoiseController::NoiseController( const PropNode& props, Params& par, Model& model, const Location& loc ) :
	Controller( props, par, model, loc ),
	random_seed( props.get< unsigned int >( "random_seed", 123 ) ),
	rng_( random_seed )
	{
		INIT_PROP( props, base_noise, 0 );
		INIT_PROP( props, proportional_noise, 0 );
	}

	bool NoiseController::ComputeControls( Model& model, double timestamp )
	{
		for ( auto& a : model.GetActuators() )
			a->AddInput( rng_.norm( 0.0, proportional_noise * a->GetInput() + base_noise ) );
		return false;
	}

	String NoiseController::GetClassSignature() const
	{
		return String( "N" );
	}
}
