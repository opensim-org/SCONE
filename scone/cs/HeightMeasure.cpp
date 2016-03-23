#include "stdafx.h"

#include "HeightMeasure.h"
#include "scone/sim/Model.h"
#include "scone/core/Log.h"
#include "scone/core/Profiler.h"

namespace scone
{
	namespace cs
	{
		HeightMeasure::HeightMeasure( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area ) :
		Measure( props, par, model, area ),
		m_pTargetBody( nullptr ),
		m_JumpStartHeight( 0 ),
		m_JumpState( InitialState )
		{
			INIT_PROPERTY( props, target_body, String("") );
			INIT_PROPERTY( props, use_average_height, false );
			INIT_PROPERTY( props, terminate_on_peak, true );
			INIT_PROPERTY( props, termination_height, 0.5 );
			INIT_PROPERTY( props, max_admitted_counter_height, 0.0 );
			INIT_PROPERTY( props, ignore_time, 0.05 );
			INIT_PROPERTY( props, upward_velocity_threshold, 0.05 );
			INIT_PROPERTY( props, downward_velocity_threshold, -0.05 );

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

			// update the state
			if ( timestamp > ignore_time )
			{
				switch ( m_JumpState )
				{
				case InitialState:
					if ( vel <= downward_velocity_threshold )
						m_JumpState = DownState;
					else if ( vel >= upward_velocity_threshold )
						return RequestTermination; // moving up too soon
					break;
				case DownState:
					m_JumpStartHeight = std::min( m_JumpStartHeight, pos );
					if ( vel >= upward_velocity_threshold )
						m_JumpState = UpState;
					break;
				case UpState:
					if ( terminate_on_peak && vel < 0.0 )
						return RequestTermination;
					break;
				}
			}

			return SuccessfulUpdate;
		}

		double HeightMeasure::GetResult( sim::Model& model )
		{
			if ( m_JumpState == InitialState )
				return 100 * ( termination_height - 1 ) * m_InitialHeight; // same score as just falling

			// compute admissible start height
			double lo_height = std::max( m_InitialHeight - max_admitted_counter_height, m_JumpStartHeight );
			double hi_height = terminate_on_peak ? m_Height.GetLatest() : m_Height.GetHighest();

			GetReport().Set( "Initial", m_InitialHeight );
			GetReport().Set( "JumpStartHeight", m_JumpStartHeight );
			GetReport().Set( "lo_height", lo_height );
			GetReport().Set( "hi_height", hi_height );

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
