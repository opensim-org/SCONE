#include "stdafx.h"
#include "JumpingMeasure.h"
#include "..\sim\Model.h"

namespace scone
{
	namespace cs
	{
		bool JumpingMeasure::UpdateControls( sim::Model& model, double timestamp )
		{
			if ( timestamp == 0.0 )
				m_Initial = GetCurrentComHeight( model );

			return true;
		}

		double JumpingMeasure::GetResult( sim::Model& model )
		{
			return -100 * (GetCurrentComHeight( model ) - m_Initial);
		}

		void JumpingMeasure::ProcessParameters( opt::ParamSet& par )
		{
		}

		double JumpingMeasure::GetCurrentComHeight( sim::Model& model )
		{
			double g = -model.GetGravity()[1];
			double comHeight = model.GetComPos()[1] + pow( model.GetComVel()[1], 2.0 ) / ( 2.0 * g );
			return comHeight;
		}
	}
}
