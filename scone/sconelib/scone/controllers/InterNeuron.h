#pragma once

#include "Neuron.h"

namespace scone
{
	struct InterNeuron : public Neuron
	{
		InterNeuron() : Neuron(), offset_() {}
		InterNeuron( const PropNode& pn, Params& par, Model& model, NeuralController& controller, const Locality& locality );
		double GetOutput() const override;
		void AddInput( double weight, Neuron* input ) { inputs_.emplace_back( weight, input ); }
		std::vector< std::pair< double, Neuron* > > inputs_;
		double offset_;
	};
}
