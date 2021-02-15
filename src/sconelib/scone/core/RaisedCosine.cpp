#include "RaisedCosine.h"

#include "spot/par_tools.h"
#include "xo/numerical/constants.h"
#include <cmath>

using std::cos;
constexpr auto pi = xo::constantsd::pi;

namespace scone
{
	RaisedCosine::RaisedCosine( const PropNode& pn, Params& par ) :
		INIT_PAR_MEMBER( pn, par, amplitude, 1 ),
		INIT_PAR_MEMBER( pn, par, mean, 0 ),
		INIT_PAR_MEMBER( pn, par, half_width, 1.0 ),
		INIT_PAR_MEMBER( pn, par, offset, 0.0 )
	{}

	Real RaisedCosine::GetValue( Real x )
	{
		if (mean - half_width <= x && x <= mean + half_width)
			return offset + amplitude / 2
				* (1 + cos( ( x - mean ) * pi() / half_width ));
		else
			return offset;
	}

	Real RaisedCosine::GetDerivativeValue( Real x )
	{
		if (mean - half_width <= x && x <= mean + half_width)
			return -(amplitude * pi() ) / ( 2 * half_width )
				* sin( ( x -  mean ) * pi() / half_width );
		else
			return 0;

	}
}
