/*
** NoiseController.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once
#include "Controller.h"
#include "xo/numerical/random.h"

namespace scone
{
	/// Controller that adds random signals to actuators.
	/// For each controller timestep, a random noise is sampled from a normal distribution
	/// with a standard deviation of: base_noise + current_signal * proportional_noise.
	class NoiseController : public Controller
	{
	public:
		NoiseController( const PropNode& props, Params& par, Model& model, const Location& loc );
		virtual ~NoiseController() {}

		/// Standard deviation of the normal distribution; default = 0.
		double base_noise;

		/// Proportional standard deviation of the normal distribution; default = 0.
		double proportional_noise;

		/// Random seed for noise sampling; default = 123.
		unsigned int random_seed;

	protected:
		virtual bool ComputeControls( Model& model, double timestamp ) override;
		virtual String GetClassSignature() const override;

		xo::random_number_generator rng_;
	};
}
