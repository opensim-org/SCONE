#include "BalanceMeasure.h"
#include "../model/Model.h"

namespace scone
{
	
	BalanceMeasure::BalanceMeasure( const PropNode& props, Params& par, Model& model, const Location& loc ) :
	Measure( props, par, model, loc )
	{
		INIT_PROP( props, termination_height, 0.5 );

		m_InitialHeight = model.GetComPos()[ 1 ];
	}

	bool BalanceMeasure::UpdateMeasure( const Model& model, double timestamp )
	{
		double pos = model.GetComPos()[ 1 ];
		double vel = model.GetComVel()[ 1 ];

		// check if the height is still high enough
		if ( pos < termination_height * m_InitialHeight )
			return true;

		return false;
	}

	double BalanceMeasure::GetResult( Model& model )
	{
		return std::max( 0.0, ( model.GetSimulationEndTime() - model.GetTime() ) / model.GetSimulationEndTime() );
	}

	String BalanceMeasure::GetClassSignature() const
	{
		return "B";
	}
}
