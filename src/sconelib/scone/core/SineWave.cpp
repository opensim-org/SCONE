#include "SineWave.h"

#include "spot/par_tools.h"
#include "xo/numerical/constants.h"
#include <cmath>

namespace scone
{
	SineWave::SineWave( const PropNode& pn, Params& par ) :
		INIT_PAR_MEMBER( pn, par, amplitude_, 0.5 ),
		INIT_PAR_MEMBER( pn, par, frequency_, 1.0 ),
		INIT_PAR_MEMBER( pn, par, phase_, 0.0 ),
		INIT_PAR_MEMBER( pn, par, offset_, 0.5 )
	{}

	Real SineWave::GetValue( Real x )
	{
		return amplitude_ * std::sin( xo::constantsd::two_pi() * frequency_ * x + phase_ ) + offset_;
	}
}
