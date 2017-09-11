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
		Neuron() : output_() {}
		virtual ~Neuron() {}
		virtual activation_t GetOutput() const = 0;
		virtual string GetName( bool mirrored ) const { return ""; }
		double ActivationFunction( double input ) const;
		mutable double output_;
	};
}
