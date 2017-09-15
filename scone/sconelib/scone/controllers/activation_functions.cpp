#include "activation_functions.h"
#include "../core/Exception.h"

namespace scone
{
	
	activation_func_t GetActivationFunction( const String& name )
	{
		if ( name == "rectifier" ) return rectifier;
		else if ( name == "soft_plus" ) return soft_plus;
		else if ( name == "linear" ) return linear;
		else SCONE_THROW( "Unknown activation function: " + name );
	}

	double rectifier( double input )
	{
		return std::max( 0.0, input );
	}

	double soft_plus( double input )
	{
		const double scale = 0.02 / std::log( 2 );
		return scale * std::log( 1 + std::exp( input / scale ) );
	}

	double linear( double input )
	{
		return input;
	}

}
