#include "Neuron.h"
#include <algorithm>
#include <cmath>

namespace scone
{
	double Neuron::ActivationFunction( double input ) const
	{
		//return std::max( 0.0, input );
		const double scale = 0.02 / std::log( 2 );
		return scale * std::log( 1 + std::exp( input / scale ) );
	}
}
