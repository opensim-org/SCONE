#include "PatternNeuron.h"
#include "NeuralController.h"
#include "../model/Model.h"
#include <cmath>
#include "spot/par_tools.h"
#include "scone/core/string_tools.h"

namespace scone
{
	PatternNeuron::PatternNeuron( const PropNode& pn, Params& par, NeuralController& nc, int index, bool mirrored ) :
	Neuron( pn, stringf( "CPG%d", index ), index, mirrored ? RightSide : LeftSide, "linear" ),
	model_( nc.GetModel() ),
	mirrored_( mirrored )
	{
		side_ = mirrored ? RightSide : LeftSide;

		period_ = par.get( "CPG.period", pn[ "period" ] );
		auto amount = pn.get< int >( "amount" );

		ScopedParamSetPrefixer pf( par, "CPG." );

		auto tname = "t" + to_str( index );
		if ( pn.has_key( tname ) )
			t0_ = par.get( tname, pn[ tname ] );
		else t0_ = par.get( tname, ( index + 1 ) * period_ / ( amount + 1 ), period_ / ( amount + 1 ) );
		if ( mirrored )
			t0_ = fmod( t0_ + period_ / 2, period_ );

		auto wname = "w" + to_str( index );
		if ( pn.has_key( wname ) )
			width_ = par.get( wname, pn[ wname ] );
		else width_ = par.try_get( wname, pn, "width", period_ / ( amount + 1 ) );

		auto c = width_ / 2.35482;
		beta_ = 1 / ( 2 * c * c );
	}

	scone::activation_t PatternNeuron::GetOutput( double offset ) const
	{
		auto t = xo::wrapped( model_.GetTime() - t0_, -0.5 * period_, 0.5 * period_ );
		return output_ = exp( -beta_ * t * t );
	}
}
