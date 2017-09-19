#pragma once
#include "Neuron.h"

namespace scone
{
	class PatternNeuron : public Neuron
	{
	public:
		PatternNeuron( const PropNode& pn, Params& par, NeuralController& nc );
		virtual ~PatternNeuron() {}

		virtual activation_t GetOutput() const override;

	private:
		double sigma_;
		double t0_;
		double period_;
		const class Model& model_;
	};
}
