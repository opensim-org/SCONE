/*
** SensorNeuron.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once
#include "Neuron.h"

namespace scone
{
	struct SensorNeuron : public Neuron
	{
		SensorNeuron( const PropNode& pn, Params& par, NeuralController& nc, const String& name, index_t idx, Side side, const String& act_func );
		double GetOutput( double offset = 0.0 ) const override;
		virtual string GetName( bool mirrored ) const override;
		virtual string GetParName() const override;

		SensorDelayAdapter* input_sensor_;
		TimeInSeconds delay_;
		bool use_sample_delay_;
		int sample_delay_frames_;
		int sample_delay_window_;
		double sensor_gain_;
		string source_name_;
		string type_;
	};
}
