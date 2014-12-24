#include "stdafx.h"
#include "JumpingMeasure.h"
#include "..\sim\Model.h"

namespace scone
{
	namespace cs
	{
		bool JumpingMeasure::UpdateControls( sim::Model& model, double timestamp )
		{
			return true;
		}

		double JumpingMeasure::GetResult( sim::Model& model )
		{
			// TODO: add COM velocity
			return -model.GetComPos().y;
		}

		void JumpingMeasure::ProcessParameters( opt::ParamSet& par )
		{
		}
	}
}
