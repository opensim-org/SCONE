#pragma once

#include "InterNeuron.h"

namespace scone
{
	struct MotorNeuron : public Neuron
	{
		MotorNeuron( const PropNode& pn, Params& par, NeuralController& nc, const string& muscle, index_t idx, Side side, const string& act_func = "rectifier" );
		void UpdateActuator();
	};
}
