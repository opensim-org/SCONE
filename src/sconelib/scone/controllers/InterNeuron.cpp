#include "InterNeuron.h"

#include "scone/core/string_tools.h"
#include "scone/model/Locality.h"
#include "spot/par_tools.h"
#include "NeuralController.h"
#include "../model/Model.h"
#include "../model/Actuator.h"
#include "../model/Side.h"
#include "xo/string/dictionary.h"
#include "scone/model/Muscle.h"
#include "xo/string/string_tools.h"

namespace scone
{
	InterNeuron::InterNeuron( const PropNode& pn, Params& par, const string& layer, Index idx, Side side, const string& act_func ) :
	Neuron( pn, idx, side, act_func )
	{
		ScopedParamSetPrefixer ps( par, layer + stringf( "_%d.", idx ) );
		name_ = GetSidedName( layer + stringf( "_%d", idx ), side );

		INIT_PAR( pn, par, width_, 0.0 );
		use_distance_ = act_func == "gaussian"; // TODO: neater

		offset_ = par.try_get( "C0", pn, "offset", 0.0 );
	}

	double InterNeuron::GetOutput() const
	{
		if ( use_distance_ )
		{
			double dist = 0.0;
			for ( auto& i : inputs_ )
				dist += xo::squared( i.neuron->GetOutput() - i.offset );
			dist = sqrt( dist );

			return output_ = offset_ + gaussian_width( dist, width_ );
		}
		else
		{
			activation_t value = offset_;
			for ( auto& i : inputs_ )
			{
				auto input = i.gain * i.neuron->GetOutput();
				i.contribution += abs( input );
				value += input;
			}

			return output_ = activation_function( value );
		}
	}
}
