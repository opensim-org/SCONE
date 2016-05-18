#include "JointLoadMeasure.h"

#include "../sim/Model.h"

namespace scone
{
	namespace cs
	{
		JointLoadMeasure::JointLoadMeasure( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area ) :
		Measure( props, par, model, area ),
		joint( *FindByName( model.GetJoints(), props.GetStr( "joint" ) ) ),
		load_penalty( props )
		{
			INIT_PROPERTY( props, method, 1 );
		}

		double JointLoadMeasure::GetResult( sim::Model& model )
		{
			return load_penalty.GetAverage();
		}

		scone::sim::Controller::UpdateResult JointLoadMeasure::UpdateAnalysis( const sim::Model& model, double timestamp )
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
}
