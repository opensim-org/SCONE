#include "stdafx.h"
#include "GaitMeasure.h"
#include "../sim/Model.h"

namespace scone
{
	namespace cs
	{
		GaitMeasure::GaitMeasure( const PropNode& props, opt::ParamSet& par, sim::Model& model ) :
		Measure( props, par, model )
		{
			INIT_FROM_PROP( props, termination_height, 0.5 );

			m_InitialComPos = model.GetComPos();
		}

		GaitMeasure::~GaitMeasure()
		{
		}

		void GaitMeasure::UpdateControls( sim::Model& model, double timestamp )
		{
			// check if this is a new step
			if ( model.GetIntegrationStep() == model.GetPreviousIntegrationStep() )
				return;

			// check if com is too low
			Vec3 com = model.GetComPos();

			if ( com.y < termination_height * m_InitialComPos.y )
				SetTerminationRequest();
		}

		double GaitMeasure::GetResult( sim::Model& model )
		{
			return 100 * (model.GetComPos().x - m_InitialComPos.x);
		}

		scone::String GaitMeasure::GetSignature()
		{
			return "Gait";
		}
	}
}
