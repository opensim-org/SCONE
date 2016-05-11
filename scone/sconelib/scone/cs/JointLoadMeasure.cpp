#include "JointLoadMeasure.h"

#include "../sim/Model.h"

namespace scone
{
	namespace cs
	{
		JointLoadMeasure::JointLoadMeasure( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area ) :
		Measure( props, par, model, area ),
		joint( *FindByName( model.GetJoints(), props.GetStr( "joint" ) ) )
		{
			INIT_PROPERTY( props, method, 0 );
		}

		double JointLoadMeasure::GetResult( sim::Model& model )
		{
			return penalty.GetAverage();
		}

		scone::sim::Controller::UpdateResult JointLoadMeasure::UpdateAnalysis( const sim::Model& model, double timestamp )
		{


			return Controller::SuccessfulUpdate;
		}

		scone::String JointLoadMeasure::GetClassSignature() const
		{
			return "";
		}

		void JointLoadMeasure::StoreData( Storage< Real >::Frame& frame )
		{
			// TODO: store joint load value
		}
	}
}
