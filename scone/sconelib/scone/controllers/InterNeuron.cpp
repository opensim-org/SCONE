#include "InterNeuron.h"

#include "scone/model/Locality.h"
#include "spot/par_tools.h"
#include "NeuralController.h"
#include "../model/Model.h"
#include "../model/Actuator.h"
#include "../model/Side.h"

namespace scone
{
	InterNeuron::InterNeuron( const PropNode& pn, Params& par, NeuralController& nc, const string& name ) :
	Neuron( pn, par, nc ),
	name_( name )
	{
		//ScopedParamSetPrefixer prefix( par, GetNameNoSide( name ) + "." );
		INIT_PAR_NAMED( pn, par, offset_, "C0", 0 );
		INIT_PAR( pn, par, width_, 0.0 );
		use_distance_ = pn.get< string >( "activation_function", "" ) == "gaussian"; // TODO: neater
	}

	double InterNeuron::GetOutput() const
	{
		activation_t value = offset_;

		if ( use_distance_ )
		{
			for ( auto& i : inputs_ )
				value += i.weight * flut::math::squared( i.neuron->GetOutput() - i.mean );
			value = sqrt( value );
			return output_ = gaussian_width( value, width_ );
		}
		else
		{
			for ( auto& i : inputs_ )
				value += i.weight * i.neuron->GetOutput();
		}

		return output_ = activation_function( value );
	}

	scone::string InterNeuron::GetName( bool mirrored ) const
	{
		return mirrored ? GetMirroredName( name_ ) : name_;
	}
}
