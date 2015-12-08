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

			if ( !IsActive( model, timestamp ) )
				return NoUpdate;

			double pos = m_pTargetBody ? m_pTargetBody->GetPos()[1] : model.GetComPos()[1];
			double vel = m_pTargetBody ? m_pTargetBody->GetLinVel()[1] : model.GetComVel()[1];

			// add sample
			m_Height.AddSample( timestamp, pos );

			// check if the height is still high enough
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
			// results are in cm to get nice scaling
			if ( use_average_height )
				return 100 * m_Height.GetAverage();
			else return 100 * ( m_Height.GetHighest() - m_Height.GetInitial() );
		}

		scone::String HeightMeasure::GetClassSignature() const
		{
			return "Height";
		}
	}
}
