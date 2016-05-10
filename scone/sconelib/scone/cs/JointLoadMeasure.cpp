#include "JointLoadMeasure.h"

//#include "scone/core/InitFromPropNode.h"

namespace scone
{
	namespace cs
	{
		JointLoadMeasure::JointLoadMeasure( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area ) :
		Measure( props, par, model, area )
		{
			INIT_PROPERTY( props, method, 0 );
		}

		double JointLoadMeasure::GetResult( sim::Model& model )
		{
			return penalty.GetAverage();
		}

		scone::sim::Controller::UpdateResult JointLoadMeasure::UpdateAnalysis( const sim::Model& model, double timestamp )
		{
			// TODO: calculate joint load value

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
