#include "ReactionForceMeasure.h"

#include "scone/core/HasName.h"
#include "scone/model/Model.h"

namespace scone
{
	ReactionForceMeasure::ReactionForceMeasure( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area ) :
		Measure( props, par, model, area ),
		load_penalty( props )
	{
	}

	double ReactionForceMeasure::GetResult( sim::Model& model )
	{
		return load_penalty.GetAverage();
	}

	scone::sim::Controller::UpdateResult ReactionForceMeasure::UpdateAnalysis( const sim::Model& model, double timestamp )
	{
		Real leg_load = 0.0f;
		for ( auto& leg : model.GetLegs() )
			leg_load += leg->GetContactForce().length() / model.GetBW();

		load_penalty.AddSample( timestamp, leg_load );

		return Controller::SuccessfulUpdate;
	}

	void ReactionForceMeasure::StoreData( Storage< Real >::Frame& frame )
	{
		// TODO: store joint load value
		frame[ "legs.load_penalty" ] = load_penalty.GetLatest();
	}
}
