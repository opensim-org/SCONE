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
		virtual string GetParName() const override { return GetNameNoSide( name_ ); }
		void AddInput( Neuron* input, double weight, double mean = 0.0 ) { inputs_.emplace_back( Input{ input, weight, mean } ); }
		void AddInputs( const PropNode& pn, Params& par, NeuralController& nc );
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

		enum connection_t { universal, monosynaptic, antagonistic, ipsilateral, contralateral };
	};
}
