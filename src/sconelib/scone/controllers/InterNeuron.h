#pragma once

#include "Neuron.h"

namespace scone
{
	struct InterNeuron : public Neuron
	{
		InterNeuron( const PropNode& pn, Params& par, const string& layer, index_t idx, Side side, const string& act_func );
		double GetOutput( double offset = 0.0 ) const override;

		double width_;
		bool use_distance_;
	};
}
