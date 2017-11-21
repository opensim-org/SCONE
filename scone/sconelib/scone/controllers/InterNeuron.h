#pragma once

#include "Neuron.h"

namespace scone
{
	class NeuralController;
	struct InterNeuron : public Neuron
	{
		InterNeuron( const PropNode& pn, Params& par, const string& layer, Index idx, Side side, const string& act_func );
		double GetOutput() const override;

		double width_;
		bool use_distance_;
	};
}
