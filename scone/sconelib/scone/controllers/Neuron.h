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
		Neuron( const string& name = "" ) : name_( name ), output_() {}
		virtual ~Neuron() {}
		virtual activation_t GetOutput() const = 0;

		double ActivationFunction( double input ) const;
		const String& GetName() { return name_; }
		String name_;
		String par_name_;
		mutable double output_;
	};
}
