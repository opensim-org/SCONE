#pragma once

#include "Neuron.h"

namespace scone
{
	class NeuralController;
	struct InterNeuron : public Neuron
	{
		InterNeuron( const PropNode& pn, Params& par, NeuralController& nc, const string& name );
		double GetOutput() const override;
		virtual string GetName( bool mirrored ) const override;
		void AddInput( Neuron* input, double weight, double mean = 0.0 ) { inputs_.emplace_back( Input{ input, weight, mean } ); }
		virtual size_t GetInputCount() override { return inputs_.size(); }

		struct Input {
			Neuron* neuron;
			double weight;
			double mean;
		};

		double width_;
		bool use_distance_;
		std::vector< Input > inputs_;
		string name_;
	};
}
