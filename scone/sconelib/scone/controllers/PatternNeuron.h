#pragma once
#include "Neuron.h"

namespace scone
{
	class PatternNeuron : public Neuron
	{
	public:
		PatternNeuron( const PropNode& pn, Params& par, NeuralController& nc, const String& name );
		virtual ~PatternNeuron() {}

		virtual activation_t GetOutput() const override;

		string name_;

	private:
		double sigma_;
		double t0_;
		double period_;
		const class Model& model_;
	};
}
