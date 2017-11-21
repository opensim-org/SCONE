#pragma once

#include "InterNeuron.h"

namespace scone
{
	struct MotorNeuron : public Neuron
	{
		MotorNeuron( const PropNode& pn, Params& par, NeuralController& nc, const string& muscle, Index idx, Side side, const string& act_func = "rectifier" );
		void UpdateActuator();
		Actuator* actuator_;
	};
}
