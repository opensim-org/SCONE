#include "JointLoadMeasure.h"

#include "scone/model/Model.h"

namespace scone
{
	JointLoadMeasure::JointLoadMeasure( const PropNode& props, ParamSet& par, Model& model, const Locality& area ) :
		Measure( props, par, model, area ),
		joint( *FindByName( model.GetJoints(), props.get< String >( "joint" ) ) ),
		load_penalty( props )
	{
		INIT_PROPERTY( props, method, 1 );
	}

	double JointLoadMeasure::GetResult( Model& model )
	{
		return load_penalty.GetAverage();
	}

	scone::Controller::UpdateResult JointLoadMeasure::UpdateAnalysis( const Model& model, double timestamp )
	{
		joint_load = joint.GetLoad();
		load_penalty.AddSample( timestamp, joint_load );

		return Controller::SuccessfulUpdate;
	}

	scone::String JointLoadMeasure::GetClassSignature() const
	{
		return "";
	}

	void JointLoadMeasure::StoreData( Storage< Real >::Frame& frame )
	{
		// TODO: store joint load value
		frame[ joint.GetName() + ".load_penalty" ] = load_penalty.GetLatest();
	}
}
