#pragma once

#include "Neuron.h"

namespace scone
{
	class NeuralController;
	struct InterNeuron : public Neuron
	{
		InterNeuron( const PropNode& pn, Params& par, const string& layer, Index idx, Side side, const string& act_func );
		double GetOutput() const override;
		virtual string GetName( bool mirrored ) const override;
		virtual string GetParName() const override { return GetNameNoSide( name_ ); }

		void AddInput( Neuron* input, double gain, double mean = 0.0  ) { inputs_.emplace_back( input, gain, mean ); }
		void AddInputs( const PropNode& pn, Params& par, NeuralController& nc );
		virtual size_t GetInputCount() override { return inputs_.size(); }

		struct Input {
			Input( Neuron* n, double g, double c ) : neuron( n ), gain( g ), center( c ), contribution( 0 ) {}
			Neuron* neuron;
			double gain;
			double center;
			mutable double contribution = 0.0;
		};

		double width_;
		bool use_distance_;

		std::vector< Input > inputs_;
		string name_;

		enum connection_t { bilateral, monosynaptic, antagonistic, ipsilateral, contralateral };
	};
}
