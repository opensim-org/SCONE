/*
** BodyMeasure.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "Measure.h"
#include "RangePenalty.h"
#include "scone/core/Angle.h"
#include "scone/model/Dof.h"

namespace scone
{
	/// Measure for penalizing when points on bodies go out of a specific range.
	/// Supports penalties based on position, velocity, and acceleration.
	class BodyMeasure : public Measure
	{
	public:
		BodyMeasure( const PropNode& props, Params& par, const Model& model, const Location& loc );
		virtual double ComputeResult( const Model& model ) override;

		/// Body to which to apply the penalty to.
		const Body& body;

		/// Offset [m] of the point of the body to measure, relative to origin; default = [ 0 0 0 ].
		Vec3 offset;

		/// Weights to apply to each axis; default = [ 0 0 0 ].
		Vec3 axes_weights;

		/// Offset is measured relative to model COM; default = false.
		bool relative_to_model_com;

		/// Penalty for when the DOF position [deg] is out of range.
		RangePenalty<Real> position;

		/// Penalty for when the DOF velocity [deg/s] is out of range.
		RangePenalty<Real> velocity;

		/// Penalty for when the DOF limit force [N] is out of range.
		RangePenalty<Real> acceleration;

	protected:
		virtual bool UpdateMeasure( const Model& model, double timestamp ) override;
		virtual String GetClassSignature() const override;
		virtual void StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags ) const override;

	private:
		int range_count;
	};
}
