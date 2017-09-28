#include "Neuron.h"

#include "activation_functions.h"
#include "NeuralController.h"
#include "spot/par_tools.h"

namespace scone
{
	Neuron::Neuron( const PropNode& pn, Params& par, const String& default_activation ) :
	output_(),
	side_( NoSide )
	{
		activation_function = GetActivationFunction( pn.get< string >( "activation", default_activation ) );
	}
}
