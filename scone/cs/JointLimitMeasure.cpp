#include "stdafx.h"
#include "JointLimitMeasure.h"

namespace scone
{
	namespace cs
	{
		JointLimitMeasure::JointLimitMeasure( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area ) :
		Measure( props, par, model, area )
		{
		}

		JointLimitMeasure::~JointLimitMeasure()
		{
		}

		void JointLimitMeasure::UpdateControls( sim::Model& model, double timestamp )
		{
			SCONE_THROW_NOT_IMPLEMENTED;
		}

		double JointLimitMeasure::GetResult( sim::Model& model )
		{
			SCONE_THROW_NOT_IMPLEMENTED;
		}

		scone::String JointLimitMeasure::GetSignature()
		{
			return "JLM";
		}
	}
}
