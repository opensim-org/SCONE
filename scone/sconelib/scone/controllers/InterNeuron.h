#pragma once

#include "Neuron.h"

namespace scone
{
	struct InterNeuron : public Neuron
	{
		InterNeuron( const string& name = "" ) : name_( name ), offset_() {}
		InterNeuron( const PropNode& pn, Params& par, Model& model, NeuralController& controller, const Locality& locality );
		double GetOutput() const override;
		virtual string GetName() const override { return name_; }
		void AddInput( double weight, Neuron* input ) { inputs_.emplace_back( weight, input ); }
		std::vector< std::pair< double, Neuron* > > inputs_;
		double offset_;
		string name_;
	};
}
