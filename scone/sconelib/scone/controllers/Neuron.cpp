#include "Neuron.h"

#include "activation_functions.h"
#include "NeuralController.h"
#include "spot/par_tools.h"

namespace scone
{
	Neuron::Neuron( const PropNode& pn, Params& par, const NeuralController& nc ) : output_()
	{
		INIT_PAR( pn, par, offset_, 0.0 );
		auto str = pn.get< string >( "activation_function", "" );
		activation_function = str.empty() ? nc.activation_function : GetActivationFunction( str );
	}
}
