/*
** PerturbationController.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "scone/core/types.h"
#include "scone/controllers/Controller.h"
#include "scone/core/PropNode.h"
#include "scone/optimization/Params.h"
#include "scone/core/Vec3.h"
#include <random>
#include "xo/numerical/bounds.h"
#include "xo/numerical/random.h"

namespace scone
{
	/// Controller that generates external perturbations to a specific body.
	class PerturbationController : public Controller
	{
	public:
		PerturbationController( const PropNode& props, Params& par, Model& model, const Location& target_area );
		virtual ~PerturbationController() {}

		/// Name of the body to which to apply the external perturbation to.
		Body& body;

		/// Local position at which to apply the perturbation; default = [ 0 0 0 ].
		Vec3 position_offset;

		// Perturbation force to apply; default = [ 0 0 0 ].
		Vec3 force;

		/// Perturbation moment to apply; default = [ 0 0 0 ].
		Vec3 moment;

		/// Random seed used for the perturbation sequence; default = time dependent.
		unsigned int random_seed;

		/// Fixed time [s] between two perturbations; default 2.
		xo::bounds< TimeInSeconds > interval;

		/// Duration of the perturbation; default = 0.
		xo::bounds< TimeInSeconds > duration;

		virtual void StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags ) const override {}
		virtual bool ComputeControls( Model& model, double timestamp ) override;

		// must be active even before start_time / after stop_time, so that perturbations can be turned off
		virtual bool IsActive( const Model& model, double time ) override { return !disabled_; }

	protected:
		virtual String GetClassSignature() const override;

	private:
		struct Perturbation {
			TimeInSeconds start;
			TimeInSeconds stop;
			Vec3 force;
			Vec3 moment;
			bool is_active( TimeInSeconds t ) const { return t >= start && t < stop; }
		};

		void AddPerturbation();
		std::vector< Perturbation > perturbations;

		xo::random_number_generator rng_;

		bool active_;
		Vec3 current_force;
		Vec3 current_moment;
		TimeInSeconds next_perturbation;
	};
}
