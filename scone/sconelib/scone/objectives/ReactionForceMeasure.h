#pragma once

#include "Measure.h"
#include "RangePenalty.h"

namespace scone
{
	class ReactionForceMeasure : public Measure
	{
	public:
		ReactionForceMeasure( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area );
		virtual ~ReactionForceMeasure() {}

		virtual double GetResult( sim::Model& model ) override;
		virtual UpdateResult UpdateAnalysis( const sim::Model& model, double timestamp ) override;

	protected:
		virtual void StoreData( Storage< Real >::Frame& frame ) override;

	private:
		RangePenalty< Real > load_penalty;
	};
}
