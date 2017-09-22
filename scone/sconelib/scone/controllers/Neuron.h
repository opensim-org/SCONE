#pragma once

#include "scone/core/types.h"
#include "scone/core/PropNode.h"
#include "scone/optimization/Params.h"
#include "scone/core/HasData.h"
#include "activation_functions.h"
#include "../model/Side.h"

namespace scone
{
	class NeuralController;
	class SensorDelayAdapter;
	using activation_t = double;

	struct Neuron
	{
		Neuron( const PropNode& pn, Params& par, const NeuralController& nc );
		virtual ~Neuron() {}
		virtual size_t GetInputCount() { return 0; }
		virtual activation_t GetOutput() const = 0;
		virtual string GetName( bool mirrored ) const { return ""; }
		virtual string GetParName() const { return ""; }

		Side GetSide( bool mirrored = false ) { return mirrored ? GetMirroredSide( side_ ) : side_; }

		Side side_;
		double offset_;
		mutable double output_;
		activation_func_t activation_function;
	};
}
