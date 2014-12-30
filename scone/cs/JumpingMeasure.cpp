#include "stdafx.h"
#include "JumpingMeasure.h"
#include "..\sim\Model.h"
#include "..\core\Log.h"

namespace scone
{
	namespace cs
	{
		bool JumpingMeasure::UpdateControls( sim::Model& model, double timestamp )
		{
			double g = -model.GetGravity()[1];
			double comPos = model.GetComPos()[1];
			double comVel = model.GetComVel()[1];
			double comHeight = comPos + pow( comVel, 2.0 ) / ( 2.0 * g );

			// record initial height or update best
			if ( timestamp == 0.0 )
				m_Initial = comHeight;

			SCONE_LOG( "time=" << timestamp << " comHeight=" << comVel << " best=" << m_Best );
			m_Best = std::max( m_Best, comHeight );

			// check if going upward
			if ( timestamp > 0.1 && comVel > 0.1 )
				m_Upward = true;

			if ( m_Upward && comVel < 0.0 )
				model.RequestTermination();

			return true;
		}

		double JumpingMeasure::GetResult( sim::Model& model )
		{
			return -100 * ( m_Best - m_Initial );
		}

		void JumpingMeasure::ProcessParameters( opt::ParamSet& par )
		{
		}

		void JumpingMeasure::Initialize( sim::Model& model )
		{
			m_Upward = false;
			m_Best = m_Initial = -999.999;
		}

	}
}
