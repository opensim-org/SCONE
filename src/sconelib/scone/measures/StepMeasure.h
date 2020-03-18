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
	/// Measure for locomotion to enforce a predefined step length.
	/** Example:
	\verbatim
	StepMeasure {
		step_length { min = 0.1 max = 0.4 abs_penalty = 1 }
	}
	\endverbatim
	*/
	class StepMeasure : public Measure
	{
	public:
		StepMeasure( const PropNode& props, Params& par, const Model& model, const Location& loc );

		/// Penalty for step length [m] when out of range.
		RangePenalty<double> step_length;

		/// Load threshold for step detection; default = 0.2.
		Real load_threshold;

		/// Minimum duration [s] of a step, used for step detection; default = 0.2.
		Real min_step_duration;

		/// Number of initial steps of which the velocity is disregarded in the final measure; default = 2.
		int initiation_steps;

		virtual bool UpdateMeasure( const Model& model, double timestamp ) override;
		virtual double ComputeResult( const Model& model ) override;

	protected:
		virtual String GetClassSignature() const override;
		virtual void StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags ) const override;

	private:
		Storage<Real> stored_data_;
	};
}
