#pragma once

#include "Neuron.h"

namespace scone
{
	struct MotorNeuron
	{
		MotorNeuron( Neuron* neuron, Actuator* act ) : input_( neuron ), output_( act ) {}
		void UpdateActuator();
		Neuron* input_;
		Actuator* output_;
	};
}
