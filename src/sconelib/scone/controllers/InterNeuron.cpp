/*
** InterNeuron.cpp
**
** Copyright (C) 2013-2018 Thomas Geijtenbeek. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "InterNeuron.h"

#include "scone/core/string_tools.h"
#include "scone/model/Location.h"
#include "spot/par_tools.h"
#include "NeuralController.h"
#include "scone/model/Model.h"
#include "scone/model/Actuator.h"
#include "scone/model/Side.h"
#include "xo/string/dictionary.h"
#include "scone/model/Muscle.h"
#include "xo/string/string_tools.h"

namespace scone
{
	InterNeuron::InterNeuron( const PropNode& pn, Params& par, const string& layer, index_t idx, Side side, const string& act_func ) :
	Neuron( pn, GetSidedName( layer + stringf( "_%d", idx ), side ), idx, side, act_func )
	{
		ScopedParamSetPrefixer ps( par, layer + stringf( "_%d.", idx ) );

		INIT_PAR( pn, par, width_, 0.0 );
		use_distance_ = act_func == "gaussian"; // TODO: neater

		offset_ = par.try_get( "C0", pn, "offset", 0.0 );
	}

	double InterNeuron::GetOutput( double offset ) const
	{
		if ( use_distance_ )
		{
			double dist = 0.0;
			for ( auto& i : inputs_ )
				dist += xo::squared( i.neuron->GetOutput( i.offset ) - i.offset - offset );
			dist = sqrt( dist );

			return output_ = offset_ + gaussian_width( dist, width_ );
		}
		else
		{
			activation_t value = offset_ + offset;
			for ( auto& i : inputs_ )
			{
				auto input = i.gain * i.neuron->GetOutput( i.offset );
				i.contribution += abs( input );
				value += input;
			}

			return output_ = activation_function( value );
		}
	}
}
