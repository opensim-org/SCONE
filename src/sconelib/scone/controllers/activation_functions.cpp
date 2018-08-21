#include "activation_functions.h"
#include "scone/core/Exception.h"

namespace scone
{
	activation_func_t GetActivationFunction( const String& name )
	{
		if ( name == "rectifier" ) return rectifier;
		else if ( name == "soft_plus" ) return soft_plus;
		else if ( name == "linear" ) return linear;
		else if ( name == "gaussian" ) return gaussian;
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

	double gaussian( double input )
	{
		return exp( -( input * input ) );
	}

	double gaussian_width( double input, double width )
	{
		auto c = width / 2.35482;
		auto b = 1 / ( 2 * c * c );
		return exp( -b * input * input );
	}
}
