#include "stdafx.h"
#include "JumpingMeasure.h"
#include "..\sim\Model.h"
#include "..\core\Log.h"

namespace scone
{
	namespace cs
	{
		JumpingMeasure::JumpingMeasure() :
		m_pTargetBody( nullptr ),
		m_LastStep( size_t( -1 ) )
		{
		}

		bool JumpingMeasure::UpdateControls( sim::Model& model, double timestamp )
		{
			// check if this is a new step
			if ( model.GetStep() != m_LastStep )
				m_LastStep = model.GetStep();
			else return true;

			double g = -model.GetGravity()[1];

			double pos = m_pTargetBody ? m_pTargetBody->GetPos()[1] : model.GetComPos()[1];
			double vel = m_pTargetBody ? m_pTargetBody->GetLinVel()[1] : model.GetComVel()[1];

			double height = pos; // + pow( vel, 2.0 ) / ( 2.0 * g );

			// record initial height or update best
			if ( timestamp == 0.0 )
				m_Initial = height;

			//printf( "ct=%.5f step=%d time/step=%.5f height=%.4f\n", timestamp, model.GetStep(), timestamp / model.GetStep(), height );
			//SCONE_LOG( "time=" << timestamp << " height=" << height << " best=" << m_Best );
			m_Best = std::max( m_Best, height );

			// check if going upward
			if ( timestamp > 0.1 && vel > 0.1 )
				m_Upward = true;

			if ( m_Upward && vel < 0.0 )
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

		void JumpingMeasure::InitFromModel( sim::Model& model )
		{
			m_Upward = false;
			m_Best = m_Initial = -999.999;

			// find target body
			if ( !target_body.empty() )
				m_pTargetBody = &model.FindBody( target_body );
			else m_pTargetBody = nullptr;
		}

		void JumpingMeasure::ProcessProperties( const PropNode& props )
		{
			INIT_FROM_PROP( props, target_body, String("") );
		}
	}
}
