/*
** InterNeuron.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "Neuron.h"

namespace scone
{
	struct InterNeuron : public Neuron
	{
		InterNeuron( const PropNode& pn, Params& par, const string& layer, index_t idx, Side side, const string& act_func );
		double GetOutput( double offset = 0.0 ) const override;

		double width_;
		bool use_distance_;
	};
}
