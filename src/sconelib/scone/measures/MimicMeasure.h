/*
** MimicMeasure.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "Measure.h"
#include "scone/core/Statistic.h"
#include "xo/string/pattern_matcher.h"

namespace scone
{
	/// Measure for how well a simulation mimics data from predefined motion file.
	/// This measure only considers variable that are part of the actual state of the model,
	/// which includes the DOFs, DOF velocities, muscle activation (*.activation), and muscle fiber length (*.fiber_length)
	/// Variables that are derived from the state are not included in the measure.
	class MimicMeasure : public Measure
	{
	public:
		MimicMeasure( const PropNode& props, Params& par, const Model& model, const Location& loc );

		/// Filename of storage (sto).
		xo::path file;

		/// States to include for comparison; default = *.
		xo::pattern_matcher include_states;

		/// States to exclude for comparison; default = "".
		xo::pattern_matcher exclude_states;

		/// Use only best match instead of average match -- useful when data contains a single pose; default = false.
		bool use_best_match;

		/// Average error above which to terminate simulation early; default = 1e9;
		Real average_error_limit;

		/// Peak error above which to terminate simulation early; default = 1e9;
		Real peak_error_limit;

		virtual bool UpdateMeasure( const Model& model, double timestamp ) override;
		virtual double ComputeResult( const Model& model ) override;
		virtual void StoreData( Storage<Real>::Frame& frame, const StoreDataFlags& flags ) const override;

	protected:
		virtual String GetClassSignature() const override;
		Storage<> storage_;
		Statistic<> result_;
		std::vector< std::pair< index_t, index_t > > state_storage_map_;
		std::vector< std::pair< String, double > > channel_errors_;

	protected:
	private:
	};
}
