/*
** BodyPointReflex.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "Reflex.h"
#include "xo/numerical/filter.h"

namespace scone
{
	/// Reflex based on the value of a specific point on a body.
	/// Must be part of ReflexController.
	class BodyPointReflex : public Reflex
	{
	public:
		BodyPointReflex( const PropNode& props, Params& par, Model& model, const Location& loc );
		virtual ~BodyPointReflex() {}

		virtual void ComputeControls( double timestamp ) override;

		/// Name of the Body that is the source of this Reflex.
		String source;

		/// Offset of the body point [m] in the local coordinate frame of the body.
		Vec3 offset;

		/// Direction in which to measure position, velocity or acceleration, in global coordinate frame.
		Vec3 direction;

		/// Target position [m] of the body point; default = 0.
		Real P0;

		/// Target velocity [m/s] of the body point; default = 0.
		Real V0;

		/// Target acceleration [m/s^2] of the body point; default = 0.
		Real A0;

		/// Position feedback gain; default = 0.
		Real KP;

		/// Velocity feedback gain; default = 0.
		Real KV;

		/// Acceleration feedback gain; default = 0.
		Real KA;

		/// Constant actuation added to the reflex; default = 0.
		Real C0;

		virtual void StoreData( Storage<Real>::Frame& frame, const StoreDataFlags& flags ) const override;

	private:
		Real u_p;
		Real u_v;
		Real u_a;
		const Body& body_;
		SensorDelayAdapter& m_DelayedPos;
		SensorDelayAdapter& m_DelayedVel;
		SensorDelayAdapter& m_DelayedAcc;
	};
}
