#include "stdafx.h"

#include "HeightMeasure.h"
#include "../sim/Model.h"
#include "../core/Log.h"
#include "../core/Profiler.h"

namespace scone
{
	namespace cs
	{
		HeightMeasure::HeightMeasure( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area ) :
		Measure( props, par, model, area ),
		m_pTargetBody( nullptr )
		{
			INIT_PROPERTY( props, target_body, String("") );
			INIT_PROPERTY( props, use_average_height, false );
			INIT_PROPERTY( props, terminate_on_peak, true );
			INIT_PROPERTY( props, termination_height, 0.5 );

			m_Upward = false;
			m_Height.Reset();

			// find target body
			if ( !target_body.empty() )
				m_pTargetBody = FindByName( model.GetBodies(), target_body ).get();
			else m_pTargetBody = nullptr;
		}

		sim::Controller::UpdateResult HeightMeasure::UpdateAnalysis( const sim::Model& model, double timestamp )
		{
			SCONE_PROFILE_SCOPE;

			// make sure this is a new step
			SCONE_ASSERT( model.GetIntegrationStep() != model.GetPreviousIntegrationStep() );

			double pos = m_pTargetBody ? m_pTargetBody->GetPos()[1] : model.GetComPos()[1];
			double vel = m_pTargetBody ? m_pTargetBody->GetLinVel()[1] : model.GetComVel()[1];
			double height = pos; // + pow( vel, 2.0 ) / ( 2.0 * g );

			//SCONE_LOG( "Force: " << model.GetLegs()[ 0 ]->GetContactForce() );

			// add sample
			m_Height.AddSample( timestamp, height );

			//printf( "ct=%.5f step=%d time/step=%.5f height=%.4f\n", timestamp, model.GetStep(), timestamp / model.GetStep(), height );
			//SCONE_LOG( "time=" << timestamp << " height=" << height << " best=" << m_Best );

			// check height
			if ( pos < termination_height * m_Height.GetInitial() )
				return RequestTermination;

			// check if there's a velocity flip
			if ( terminate_on_peak )
			{
				if ( timestamp > 0.1 && vel > 0.1 )
					m_Upward = true;
				if ( m_Upward && vel < 0.0 )
					return RequestTermination;
			}

			return SuccessfulUpdate;
		}

		double HeightMeasure::GetResult( sim::Model& model )
		{
			if ( use_average_height )
				return 100 * m_Height.GetAverage();
			else return 100 * ( m_Height.GetHighest() - m_Height.GetInitial() );
		}

		scone::String HeightMeasure::GetClassSignature() const
		{
			return "Jump";
		}
	}
}
