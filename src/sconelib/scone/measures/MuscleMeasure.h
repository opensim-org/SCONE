/*
** MuscleMeasure.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "Measure.h"
#include "RangePenalty.h"
#include "scone/core/Angle.h"
#include "scone/model/Muscle.h"

namespace scone
{
	/// Measure for penalizing when DOFs go out of a specific range.
	/// Supports penalties based on DOF position, DOF velocity, and restitution force.
	class MuscleMeasure : public Measure
	{
	public:
		MuscleMeasure( const PropNode& props, Params& par, const Model& model, const Location& loc );
		virtual double ComputeResult( const Model& model ) override;

		/// Muscle to which to apply the penalty to.
		Muscle& muscle;

		/// Penalty for when the normalized muscle activation [0..1] is out of range.
		RangePenalty< double > activation;

		/// Penalty for when the normalized muscle length [L_opt] is out of range.
		RangePenalty< double > length;

		/// Penalty for when the normalized muscle contraction velocity [L_opt/s] is out of range.
		RangePenalty< double > velocity;

	protected:
		virtual bool UpdateMeasure( const Model& model, double timestamp ) override;
		virtual String GetClassSignature() const override;
		virtual void StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags ) const override;

	private:
		int range_count;
	};
}
