/*
** ExternalBalanceController.h
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

namespace scone
{
	/// Controller that balances a specific body part using external forces.
	class ExternalBalanceController : public Controller
	{
	public:
		ExternalBalanceController( const PropNode& props, Params& par, Model& model, const Location& target_area );

		/// Name of the target body.
		String name;

		/// Target angular position [rad] of the body; default = 0.
		Real angular_position_target;

		/// Torque gain for position displacement.
		Real angular_position_gain;

		/// Target angular velocity [rad/s] of the body; default = 0.
		Real angular_velocity_gain;

		/// Torque gain for velocity damping.
		Real angular_velocity_target;

		virtual bool ComputeControls( Model& model, double timestamp ) override;
		virtual void StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags ) const override;

	protected:
		virtual String GetClassSignature() const override;

	private:
		index_t state_idx;
		Body& force_body;
	};
}
