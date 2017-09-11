#include "Neuron.h"
#include <algorithm>
#include <cmath>

namespace scone
{
	double Neuron::ActivationFunction( double input ) const
	{
#if 0
		return std::max( 0.0, input );
#else
		const double scale = 0.02 / std::log( 2 );
		return scale * std::log( 1 + std::exp( input / scale ) );
#endif
	}
}
