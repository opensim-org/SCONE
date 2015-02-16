#include "stdafx.h"
#include "GaitMeasure.h"
#include "../sim/Model.h"

namespace scone
{
	namespace cs
	{


		GaitMeasure::GaitMeasure( const PropNode& props ) : Measure( props )
		{
			INIT_FROM_PROP( props, termination_height, 0.8 );
		}

		GaitMeasure::~GaitMeasure()
		{

		}

		void GaitMeasure::Initialize( sim::Model& model, opt::ParamSet& par, const PropNode& props )
		{
			Measure::Initialize( model, par, props );

			m_InitialComPos = model.GetComPos();
		}

		void GaitMeasure::UpdateControls( sim::Model& model, double timestamp )
		{
			// check if com is too low
			Vec3 com = model.GetComPos();

			if ( com.y < termination_height * m_InitialComPos.y )
				SetTerminationRequest();
		}

		double GaitMeasure::GetResult( sim::Model& model )
		{
			return 100 * (model.GetComPos().x - m_InitialComPos.x);
		}
	}
}
