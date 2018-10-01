/*
** NoiseController.h
**
** Copyright (C) 2013-2018 Thomas Geijtenbeek. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once
#include "Controller.h"
#include "xo/numerical/random.h"

namespace scone
{
	class NoiseController : public Controller
	{
	public:
		NoiseController( const PropNode& props, Params& par, Model& model, const Location& loc );
		virtual ~NoiseController() {}

		double base_noise;
		double proportional_noise;
		unsigned int random_seed;

	protected:
		virtual bool ComputeControls( Model& model, double timestamp ) override;
		virtual String GetClassSignature() const override;

		xo::random_number_generator rng_;
	};
}
