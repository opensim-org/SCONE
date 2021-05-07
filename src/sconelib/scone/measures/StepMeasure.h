/*
** StepMeasure.h
**
** Copyright (C) 2013-2021 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once
#include "Measure.h"
#include "RangePenalty.h"

namespace scone
{
	/// Measure for locomotion that penalizes stride length, stride duration, and/or stride velocity.
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

		/// Penalty for velocity [m/s] when out of range
		/// (Optional); default range [-inf, inf].
		RangePenalty<double> stride_velocity;

		/// Load threshold for step detection; default = 0.01.
		Real load_threshold;

		/// Minimum duration of the stance phase [s], shorter contacts are considered as 'bump' during the swing phase; default = 0.2.
		Real min_stance_duration_threshold;

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
