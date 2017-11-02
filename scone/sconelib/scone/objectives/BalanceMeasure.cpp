#include "BalanceMeasure.h"
#include "../model/Model.h"

namespace scone
{
	
	BalanceMeasure::BalanceMeasure( const PropNode& props, Params& par, Model& model, const Locality& area ) :
	Measure( props, par, model, area )
	{
		INIT_PROPERTY( props, termination_height, 0.5 );

		m_InitialHeight = model.GetComPos()[ 1 ];
	}

	scone::Controller::UpdateResult BalanceMeasure::UpdateMeasure( const Model& model, double timestamp )
	{
		double pos = model.GetComPos()[ 1 ];
		double vel = model.GetComVel()[ 1 ];

		// check if the height is still high enough
		if ( pos < termination_height * m_InitialHeight )
			return RequestTermination;

		return SuccessfulUpdate;
	}

	double BalanceMeasure::GetResult( Model& model )
	{
		return model.GetSimulationEndTime() - model.GetTime();
	}

	String BalanceMeasure::GetClassSignature() const
	{
		return "B";
	}
}
