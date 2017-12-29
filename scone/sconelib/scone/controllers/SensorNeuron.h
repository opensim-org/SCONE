#pragma once
#include "Neuron.h"

namespace scone
{
	struct SensorNeuron : public Neuron
	{
		SensorNeuron( const PropNode& pn, Params& par, NeuralController& nc, const String& name, Index idx, Side side, const String& act_func );
		double GetOutput() const override;
		virtual string GetName( bool mirrored ) const override;
		virtual string GetParName() const override;

		SensorDelayAdapter* input_;
		TimeInSeconds delay_;
		bool use_sample_delay_;
		int sample_delay_frames_;
		int sample_delay_window_;
		double sensor_gain_;
		string source_name_;
		string type_;
		string par_name_;
	};
}
