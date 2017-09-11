#pragma once

#include "InterNeuron.h"

namespace scone
{
	struct MotorNeuron : public InterNeuron
	{
		MotorNeuron( NeuralController& nc, Actuator* act, const string& name = "" ) : InterNeuron( nc, name ), actuator_( act ) {}
		void UpdateActuator();
		Actuator* actuator_;
	};
}
