#include "JointLoadMeasure.h"

#include "scone/model/Model.h"

namespace scone
{
	JointLoadMeasure::JointLoadMeasure( const PropNode& props, Params& par, Model& model, const Locality& area ) :
		Measure( props, par, model, area ),
		joint( *FindByName( model.GetJoints(), props.get< String >( "joint" ) ) ),
		load_penalty( props )
	{
		INIT_PROP( props, method, 1 );
	}

	double JointLoadMeasure::GetResult( Model& model )
	{
		return load_penalty.GetAverage();
	}

	bool JointLoadMeasure::UpdateMeasure( const Model& model, double timestamp )
	{
		joint_load = joint.GetLoad();
		load_penalty.AddSample( timestamp, joint_load );

		return false;
	}

	scone::String JointLoadMeasure::GetClassSignature() const
	{
		return "";
	}

	void JointLoadMeasure::StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags ) const
	{
		// TODO: store joint load value
		frame[ joint.GetName() + ".load_penalty" ] = load_penalty.GetLatest();
	}
}
