/*
** MotorNeuron.h
**
** Copyright (C) 2013-2018 Thomas Geijtenbeek. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "InterNeuron.h"

namespace scone
{
	struct MotorNeuron : public Neuron
	{
		MotorNeuron( const PropNode& pn, Params& par, NeuralController& nc, const string& muscle, index_t idx, Side side, const string& act_func = "rectifier" );
		void UpdateActuator();
	};
}
