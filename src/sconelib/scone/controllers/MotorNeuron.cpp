/*
** MotorNeuron.cpp
**
** Copyright (C) 2013-2021 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "MotorNeuron.h"

#include "scone/model/Actuator.h"
#include "scone/model/Model.h"
#include "scone/model/Muscle.h"
#include "scone/core/HasName.h"
#include "NeuralController.h"

namespace scone
{
	MotorNeuron::MotorNeuron( const PropNode& pn, Params& par, NeuralController& nc, const string& muscle, index_t idx, Side side, const string& act_func ) :
	Neuron( pn, muscle, idx, side, act_func )
	{
		muscle_ = FindByName( nc.GetModel().GetMuscles(), muscle ).get();

		ScopedParamSetPrefixer ps( par, GetParName() + "." );
		offset_ = par.try_get( "C0", pn, "offset", 0.0 );
	}

	void MotorNeuron::UpdateActuator()
	{
		auto v = GetOutput();
		muscle_->AddInput( v );
	}
}
