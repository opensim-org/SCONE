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
		m_pTargetBody( nullptr ),
		m_JumpStartHeight( 0 )
		{
			INIT_PROPERTY( props, target_body, String("") );
			INIT_PROPERTY( props, use_average_height, false );
			INIT_PROPERTY( props, terminate_on_peak, true );
			INIT_PROPERTY( props, termination_height, 0.5 );
			INIT_PROPERTY( props, max_admitted_counter_height, 0.0 );
			INIT_PROPERTY( props, ignore_time, 0.1 );
			INIT_PROPERTY( props, min_upward_velocity, 0.1 );

			m_Upward = false;
			m_Height.Reset();

			// find target body
			if ( !target_body.empty() )
				m_pTargetBody = FindByName( model.GetBodies(), target_body ).get();
			else m_pTargetBody = nullptr;

			// init start height and initial height
			m_JumpStartHeight = m_InitialHeight = m_pTargetBody ? m_pTargetBody->GetPos()[1] : model.GetComPos()[1];
		}

		sim::Controller::UpdateResult HeightMeasure::UpdateAnalysis( const sim::Model& model, double timestamp )
		{
			SCONE_PROFILE_SCOPE;

			double pos = m_pTargetBody ? m_pTargetBody->GetPos()[1] : model.GetComPos()[1];
			double vel = m_pTargetBody ? m_pTargetBody->GetLinVel()[1] : model.GetComVel()[1];

			// add sample
			m_Height.AddSample( timestamp, pos );

			// check if the height is still high enough
			if ( pos < termination_height * m_Height.GetInitial() )
				return RequestTermination;

			// check if there's a velocity flip
			if ( !m_Upward )
			{
				if ( timestamp > ignore_time && vel > min_upward_velocity )
				{
					// initiate move upward
					m_Upward = true;
					m_JumpStartHeight = pos; // set start height if lower than initial height
				}
			}
			else
			{
				if ( terminate_on_peak && vel < 0.0 )
					return RequestTermination;
			}

			return SuccessfulUpdate;
		}

		double HeightMeasure::GetResult( sim::Model& model )
		{
			// compute admissible start height
			double lo_height = std::max( m_InitialHeight - max_admitted_counter_height, m_JumpStartHeight );
			double hi_height = terminate_on_peak ? m_Height.GetLatest() : m_Height.GetHighest();

			// results are in cm to get nice scaling
			if ( use_average_height )
				return 100 * m_Height.GetAverage();
			else return 100 * ( hi_height - lo_height );
		}

		scone::String HeightMeasure::GetClassSignature() const
		{
			return "Height";
		}
	}
}
