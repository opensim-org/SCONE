#pragma once

#include "Measure.h"
#include "RangePenalty.h"

namespace scone
{
	class ReactionForceMeasure : public Measure
	{
	public:
		ReactionForceMeasure( const PropNode& props, Params& par, Model& model, const Locality& area );
		virtual ~ReactionForceMeasure() {}

		/// Penalty for reaction force
		RangePenalty< Real > load_penalty;

		virtual double GetResult( Model& model ) override;
		virtual bool UpdateMeasure( const Model& model, double timestamp ) override;

	protected:
		virtual void StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags ) const override;
	};
}
