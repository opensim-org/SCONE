/*
** NoiseController.cpp
**
** Copyright (C) 2013-2021 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "NoiseController.h"
#include "scone/model/Model.h"
#include "scone/model/Actuator.h"
#include "scone/core/string_tools.h"

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
		{
			auto noise_std = base_noise + proportional_noise * a->GetInput();
			if ( noise_std > 0.0 )
				a->AddInput( rng_.norm( 0.0, noise_std ) );
		}
		return false;
	}

	String NoiseController::GetClassSignature() const
	{
		return stringf( "N%02d", xo::round_cast<int>( 100 * proportional_noise ) );
	}
}
