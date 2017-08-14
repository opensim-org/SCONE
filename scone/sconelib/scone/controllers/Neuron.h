#pragma once

#include "scone/core/types.h"
#include "scone/core/PropNode.h"
#include "scone/optimization/Params.h"

namespace scone
{
	class SensorDelayAdapter;
	using activation_t = double;

	struct Neuron
	{
		Neuron( const PropNode& pn, Params& par, Model& model );
		void UpdateOutput();

		activation_t output_;
		std::vector< std::pair< double, activation_t* > > inputs_;
		std::vector< double > weights_;
		double offset;
	};

	struct SensorNeuron
	{
		SensorNeuron( const PropNode& pn, Params& par, Model& model );
		void UpdateOutput();

		activation_t output_;
		SensorDelayAdapter* intput_;
		TimeInSeconds delay_;
	};

	struct MotorNeuron
	{
	public:
		MotorNeuron( const PropNode& pn, Params& par, Model& model );
		void UpdateActuator();

		activation_t* input_;
		Actuator* output_;
	};
}
