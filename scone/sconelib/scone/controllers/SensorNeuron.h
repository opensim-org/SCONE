#pragma once
#include "Neuron.h"

namespace scone
{
	struct SensorNeuron : public Neuron
	{
		SensorNeuron( const PropNode& pn, Params& par, NeuralController& nc, const String& name );
		double GetOutput() const override;
		virtual string GetName( bool mirrored ) const override;

		SensorDelayAdapter* input_;
		TimeInSeconds delay_;
		double sensor_gain_;
		string source_name_;
		string type_;
		string par_name_;
	};
}
