#pragma once
#include "Neuron.h"

namespace scone
{
	struct SensorNeuron : public Neuron
	{
		SensorNeuron( Model& model, const Locality& loc, const string& type, const string& source, double delay, double offset = 0.0, bool inverted = false );
		SensorNeuron( const PropNode& pn, Params& par, Model& model, Locality locality );
		double GetOutput() const override;

		void SetInputSensor( Model& model, const string& type, const string& name, const Locality& loc );

		SensorDelayAdapter* input_;
		TimeInSeconds delay_;
		double offset_;
		double sensor_gain_;
	};
}
