#pragma once

#include "scone/core/types.h"
#include "scone/core/PropNode.h"
#include "scone/optimization/Params.h"
#include "../core/HasData.h"

namespace scone
{
	class NeuralController;
	class SensorDelayAdapter;
	using activation_t = double;

	struct Neuron
	{
		Neuron( const PropNode& pn, Params& par, Model& model, NeuralController& controller, const Locality& locality );
		void UpdateOutput();

		const string& GetName() const { return name_; }
		string name_;
		activation_t output_;
		std::vector< std::pair< double, activation_t* > > inputs_;
		std::vector< double > weights_;
		double offset_;
		Actuator* actuator_;
	};

	struct SensorNeuron
	{
		SensorNeuron( const PropNode& pn, Params& par, Model& model, Locality locality );
		void UpdateOutput();
		const string& GetName() const { return name_; }

		string name_;
		activation_t output_;
		double offset_;
		SensorDelayAdapter* input_;
		TimeInSeconds delay_;
	};
}
