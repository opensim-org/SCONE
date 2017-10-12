#include "Neuron.h"

#include "activation_functions.h"
#include "NeuralController.h"
#include "spot/par_tools.h"

namespace scone
{
	Neuron::Neuron( const PropNode& pn, Params& par, Index idx, Side s, const String& default_activation ) :
	output_(),
	index_( idx ),
	side_( s )
	{
		activation_function = GetActivationFunction( pn.get< string >( "activation", default_activation ) );
	}
}
