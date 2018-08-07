#pragma once
#include "Neuron.h"

namespace scone
{
	struct PatternNeuron : public Neuron
	{
	public:
		PatternNeuron( const PropNode& pn, Params& par, NeuralController& nc, int index, bool mirrored );
		virtual ~PatternNeuron() {}

		virtual activation_t GetOutput( double offset = 0.0 ) const override;
		virtual string GetName( bool mirrored ) const override { return name_ + ( mirrored_ ? "_r" : "_l" ); }

		bool mirrored_;

	private:
		double width_;
		double beta_;
		double t0_;
		double period_;
		const class Model& model_;
	};
}
