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
	/// Measure for penalizing specific muscle parameters.
	/** Penalties can be based on muscle activation, length and velocity. Example:
	\verbatim
	MuscleMeasure {
		muscle = gastroc
		activation { max = 0 squared_penalty = 1 } # penalize squared activation
		length { max = 1 abs_penalty = 1 } # Penalize when ( length > optimizal fiber length )
	}
	\endverbatim
	*/
	class MuscleMeasure : public Measure
	{
	public:
		MuscleMeasure( const PropNode& props, Params& par, const Model& model, const Location& loc );
		virtual double ComputeResult( const Model& model ) override;

		/// Muscle to which to apply the penalty to.
		Muscle& muscle;

		/// Penalty for when the normalized muscle activation [0..1] is out of range.
		RangePenalty<Real> activation;

		/// Penalty for when the normalized muscle length [L_opt] is out of range.
		RangePenalty<Real> length;

		/// Penalty for when the normalized muscle contraction velocity [L_opt/s] is out of range.
		RangePenalty<Real> velocity;

	protected:
		virtual bool UpdateMeasure( const Model& model, double timestamp ) override;
		virtual String GetClassSignature() const override;
		virtual void StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags ) const override;

	private:
		int range_count;
	};
}
