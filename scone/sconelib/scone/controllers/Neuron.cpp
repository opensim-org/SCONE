#include "Neuron.h"
#include <algorithm>

namespace scone
{
	double Neuron::ActivationFunction( double input ) const
	{
		return std::max( 0.0, input );
	}
}
