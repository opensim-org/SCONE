#pragma once

#include "Measure.h"
#include "scone/core/Statistic.h"
#include "xo/string/pattern_matcher.h"

namespace scone
{
	/// Measure for how well a simulation mimics data from predefined motion file.
	class MimicMeasure : public Measure
	{
	public:
		MimicMeasure( const PropNode& props, Params& par, Model& model, const Locality& area );
		virtual ~MimicMeasure() { };

		/// Filename of storage (sto).
		xo::path file;

		/// Use only best match instead of average match -- useful when data contains a single pose; default = false.
		bool use_best_match;

		/// States to include for comparison; default = *.
		xo::pattern_matcher include_states;

		/// States to exclude for comparison; default = "".
		xo::pattern_matcher exclude_states;

		virtual bool UpdateMeasure( const Model& model, double timestamp ) override;
		virtual double GetResult( Model& model ) override;
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
