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
	/// Measure for locomotion that enforces a constant stride length.
	/** Example:
	\verbatim
	StepMeasure {
		stride_length { min = 1.0 max = 1.4 abs_penalty = 1 }
	}
	\endverbatim
	*/
	class StepMeasure : public Measure
	{
	public:
		StepMeasure( const PropNode& props, Params& par, const Model& model, const Location& loc );

		/// Penalty for stride length [m] when out of range.
		RangePenalty<double> stride_length;

		/// Load threshold for step detection; default = 0.01.
		Real load_threshold;

		/// Minimum duration [s] of a step, used for step detection; default = 0.2.
		Real min_step_duration;

		/// Number of initial gait cycles of which the calculation is
		/// disregarded in the final measure; default = 1.
		int initiation_cycles;

		virtual bool UpdateMeasure( const Model& model, double timestamp ) override;
		virtual double ComputeResult( const Model& model ) override;

	protected:
		virtual String GetClassSignature() const override;

	private:
		Storage<Real> stored_data_;
	};
}
