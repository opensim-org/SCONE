#include "stdafx.h"

#include "HeightMeasure.h"
#include "../sim/Model.h"
#include "../core/Log.h"

namespace scone
{
	namespace cs
	{
		HeightMeasure::HeightMeasure( const PropNode& props ) :
		Measure( props ),
		m_pTargetBody( nullptr ),
		m_LastStep( size_t( -1 ) )
		{
			INIT_FROM_PROP( props, target_body, String("") );
			INIT_FROM_PROP( props, use_average_height, false );
			INIT_FROM_PROP( props, terminate_on_peak, true );
			INIT_FROM_PROP( props, termination_height, 0.5 );
		}

		void HeightMeasure::Initialize( sim::Model& model, opt::ParamSet& par, const PropNode& props )
		{
			Measure::Initialize( model, par, props );

			m_Upward = false;
			m_Height.Reset();
			m_LastStep = size_t( -1 );

			// find target body
			if ( !target_body.empty() )
				m_pTargetBody = &model.FindBody( target_body );
			else m_pTargetBody = nullptr;
		}

		void HeightMeasure::UpdateControls( sim::Model& model, double timestamp )
		{
			// check if this is a new step
			if ( model.GetStep() != m_LastStep )
				m_LastStep = model.GetStep();
			else return;

			double g = -model.GetGravity()[1];
			double pos = m_pTargetBody ? m_pTargetBody->GetPos()[1] : model.GetComPos()[1];
			double vel = m_pTargetBody ? m_pTargetBody->GetLinVel()[1] : model.GetComVel()[1];
			double height = pos; // + pow( vel, 2.0 ) / ( 2.0 * g );

			//SCONE_LOG( "Force: " << model.GetLegs()[ 0 ]->GetContactForce() );

			// add sample
			m_Height.AddSample( height, timestamp );

			//printf( "ct=%.5f step=%d time/step=%.5f height=%.4f\n", timestamp, model.GetStep(), timestamp / model.GetStep(), height );
			//SCONE_LOG( "time=" << timestamp << " height=" << height << " best=" << m_Best );

			// check height
			if ( pos < termination_height * m_Height.GetInitial() )
				SetTerminationRequest();

			// check if there's a velocity flip
			if ( terminate_on_peak )
			{
				if ( timestamp > 0.1 && vel > 0.1 )
					m_Upward = true;
				if ( m_Upward && vel < 0.0 )
					SetTerminationRequest();
			}
		}

		double HeightMeasure::GetResult( sim::Model& model )
		{
			if ( use_average_height )
				return 100 * m_Height.GetAverage();
			else return 100 * ( m_Height.GetHighest() - m_Height.GetInitial() );
		}
	}
}
