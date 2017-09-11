#pragma once

#include "InterNeuron.h"

namespace scone
{
	struct MotorNeuron : public InterNeuron
	{
		MotorNeuron( Actuator* act, const string& name = "" ) : InterNeuron( name ), actuator_( act ) {}
		void UpdateActuator();
		Actuator* actuator_;
	};
}
