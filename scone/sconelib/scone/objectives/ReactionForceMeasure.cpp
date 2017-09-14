#include "ReactionForceMeasure.h"

#include "scone/core/HasName.h"
#include "scone/model/Model.h"

namespace scone
{
	ReactionForceMeasure::ReactionForceMeasure( const PropNode& props, Params& par, Model& model, const Locality& area ) :
		Measure( props, par, model, area ),
		load_penalty( props )
	{
	}

	double ReactionForceMeasure::GetResult( Model& model )
	{
		return load_penalty.GetAverage();
	}

	scone::Controller::UpdateResult ReactionForceMeasure::UpdateAnalysis( const Model& model, double timestamp )
	{
		Real leg_load = 0.0f;
		for ( auto& leg : model.GetLegs() )
			leg_load += leg->GetContactForce().length() / model.GetBW();

		load_penalty.AddSample( timestamp, leg_load );

		return Controller::SuccessfulUpdate;
	}

	void ReactionForceMeasure::StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags ) const
	{
		// TODO: store joint load value
		frame[ "legs.load_penalty" ] = load_penalty.GetLatest();
	}
}
