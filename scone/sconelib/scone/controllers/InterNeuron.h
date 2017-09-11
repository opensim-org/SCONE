#pragma once

#include "Neuron.h"

namespace scone
{
	struct InterNeuron : public Neuron
	{
		InterNeuron( const string& name = "" ) : name_( name ), offset_() {}
		InterNeuron( const PropNode& pn, Params& par, Model& model, NeuralController& controller, const Locality& locality );
		double GetOutput() const override;
		virtual string GetName( bool mirrored ) const override;
		void AddInput( double weight, Neuron* input ) { inputs_.emplace_back( weight, input ); }
		virtual size_t GetInputCount() override { return inputs_.size(); }
		std::vector< std::pair< double, Neuron* > > inputs_;
		double offset_;
		string name_;
	};
}
