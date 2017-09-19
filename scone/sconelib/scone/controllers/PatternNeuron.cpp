#include "PatternNeuron.h"
#include "NeuralController.h"
#include "..\model\Model.h"
#include <cmath>
#include "flut\math\math.hpp"
#include "spot\par_tools.h"
#include "scone/core/string_tools.h"

namespace scone
{
	PatternNeuron::PatternNeuron( const PropNode& pn, Params& par, NeuralController& nc, int index, bool mirrored ) :
	Neuron( pn, par, nc ),
	model_( nc.GetModel() ),
	mirrored_( mirrored )
	{
		period_ = par.get( "CPG.period", pn[ "period" ] );
		auto amount = pn.get< int >( "amount" );

		name_ = stringf( "CPG%d", index );
		ScopedParamSetPrefixer pf( par, name_ + '.' );
		t0_ = par.get( "t0", ( index + 1 ) * period_ / ( amount + 1 ), period_ / ( amount + 1 ) );
		if ( mirrored )
			t0_ = fmod( t0_ + period_ / 2, period_ );

		INIT_PAR( pn, par, width_, period_ / ( amount + 1 ) );
		auto c = width_ / 2.35482;
		beta_ = 1 / ( 2 * c * c );
	}

	scone::activation_t PatternNeuron::GetOutput() const
	{
		auto t = flut::math::wrapped( model_.GetTime() - t0_, -0.5 * period_, 0.5 * period_ );
		return output_ = exp( -beta_ * t * t );
	}
}
