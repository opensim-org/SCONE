#pragma once

#include "Measure.h"
#include "scone/core/Statistic.h"
#include "xo/string/pattern_matcher.h"

namespace scone
{
	class MimicMeasure : public Measure
	{
	public:
		MimicMeasure( const PropNode& props, Params& par, Model& model, const Locality& area );
		virtual ~MimicMeasure() { };

		virtual bool UpdateMeasure( const Model& model, double timestamp ) override;
		virtual double GetResult( Model& model ) override;
		virtual void StoreData( Storage<Real>::Frame& frame, const StoreDataFlags& flags ) const override;

	protected:
		virtual String GetClassSignature() const override;
		Storage<> storage_;
		xo::path file_;
		Statistic<> result_;
		std::vector< std::pair< index_t, index_t > > state_storage_map_;
		std::vector< std::pair< String, double > > channel_errors_;
		xo::pattern_matcher include_states_;
		xo::pattern_matcher exclude_states_;
		bool use_best_match_;

	protected:
	private:
	};
}
