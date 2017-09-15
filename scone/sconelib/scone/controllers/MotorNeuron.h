#pragma once

#include "InterNeuron.h"

namespace scone
{
	struct MotorNeuron : public InterNeuron
	{
		MotorNeuron( const PropNode& pn, Params& par, NeuralController& nc, const string& name = "" );
		void UpdateActuator();
		Actuator* actuator_;
	};
}
