#pragma once

#include "scone/core/types.h"
#include "scone/core/PropNode.h"
#include "scone/optimization/Params.h"
#include "scone/core/HasData.h"

namespace scone
{
	class NeuralController;
	class SensorDelayAdapter;
	using activation_t = double;

	struct Neuron
	{
		Neuron( NeuralController& nc ) : output_(), controller_( nc ) {}
		virtual ~Neuron() {}
		virtual size_t GetInputCount() { return 0; }
		virtual activation_t GetOutput() const = 0;
		virtual string GetName( bool mirrored ) const { return ""; }

		mutable double output_;
		NeuralController& controller_;
	};
}
