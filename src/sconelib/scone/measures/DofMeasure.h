/*
** DofMeasure.h
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
	/// Measure that adds a penalty when a degree of freedom (DOF) is outside a specific range.
	/** Penalties can be based on DOF position, DOF velocity, and restitution force. Example:
	\verbatim
	DofMeasure {
		dof = pelvis_tilt
		position { min = -45 max = 0 abs_penalty = 10 } # Penalize leaning backwards
		velocity { min = -10 max = 10 abs_penalty = 10 } # Penalize upper body motion
	}
	\endverbatim
	*/
	class DofMeasure : public Measure
	{
	public:
		DofMeasure( const PropNode& props, Params& par, const Model& model, const Location& loc );
		virtual double ComputeResult( const Model& model ) override;

		/// Dof to which to apply the penalty to.
		Dof& dof;

		/// Optional parent dof which will be added to the dof value.
		Dof* parent;

		/// Penalty for when the DOF position [deg] is out of range.
		RangePenalty<Degree> position;

		/// Penalty for when the DOF velocity [deg/s] is out of range.
		RangePenalty<Degree> velocity;

		/// Penalty for when the DOF limit force [N] is out of range (this value is signed!).
		RangePenalty<Real> force;

	protected:
		virtual bool UpdateMeasure( const Model& model, double timestamp ) override;
		virtual String GetClassSignature() const override;
		virtual void StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags ) const override;

	private:
		int range_count;
	};
}
