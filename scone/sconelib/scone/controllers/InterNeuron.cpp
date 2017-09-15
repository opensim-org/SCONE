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
	name_( "name" )
	{
		ScopedParamSetPrefixer prefix( par, GetNameNoSide( name ) + "." );
		INIT_PAR_NAMED( pn, par, offset_, "C0", 0 );
	}

	double InterNeuron::GetOutput() const
	{
		activation_t value = offset_;
		for ( auto& i : inputs_ )
			value += i.first * i.second->GetOutput();

		return output_ = activation_function( value );
	}

	scone::string InterNeuron::GetName( bool mirrored ) const
	{
		return mirrored ? GetMirroredName( name_ ) : name_;
	}
}
