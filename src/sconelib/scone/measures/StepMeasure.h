/*
** StepMeasure.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once
#include "Measure.h"
#include "RangePenalty.h"

namespace scone
{
	/// Measure for locomotion that penalizes the stride length and/or
	/// step duration.
	/** Example:
	\verbatim
	StepMeasure {
		stride_duration { min = 0.2 max = 0.5 abs_penalty = 1 }
		stride_length { min = 1.0 max = 1.4 abs_penalty = 1 }
	}
	\endverbatim
	*/
	class StepMeasure : public Measure
	{
	public:
		StepMeasure( const PropNode& props, Params& par, const Model& model, const Location& loc );

		/// Penalty for stride length [m] when out of range
		/// (Optional); default range [-inf, inf].
		RangePenalty<double> stride_length;

		/// Penalty for stride duration [s] when out of range
		/// (Optional); default range [-inf, inf].
		RangePenalty<double> stride_duration;

		/// Load threshold for step detection; default = 0.01.
		Real load_threshold;

		/// Minimum duration threshold [s] of a stride used for step detection;
		/// default = 0.2.
		Real min_stride_duration_threshold;

		/// Number of initial gait cycles of which the calculation is
		/// disregarded in the final measure; default = 1 (>0).
		int initiation_cycles;

		virtual bool UpdateMeasure( const Model& model, double timestamp ) override;
		virtual double ComputeResult( const Model& model ) override;
		virtual String GetClassSignature() const override;

	private:
		Storage<Real> stored_data_;
	};
}
