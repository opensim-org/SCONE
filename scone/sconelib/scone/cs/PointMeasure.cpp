#include "PointYMeasure.h"
#include "scone/sim/Model.h"
#include "scone/core/Log.h"
#include "scone/core/Profiler.h"

namespace scone
{
	namespace cs
	{
		PointYMeasure::PointYMeasure( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area ) :
		Measure( props, par, model, area ),
		m_pTargetBody( nullptr )
		{
			INIT_PROPERTY( props, target_body, String("") );
            INIT_PROPERTY( props, px, 0.0 );
            INIT_PROPERTY( props, py, 0.0 );
            INIT_PROPERTY( props, pz, 0.0 );
            INIT_PROPERTY( props, squared_range_penalty, 0.0 );
			INIT_PROPERTY( props, abs_range_penalty, 0.0 );
			INIT_PROPERTY( props, squared_velocity_range_penalty, 0.0 );
			INIT_PROPERTY( props, abs_velocity_range_penalty, 0.0 );

			// find target body
			if ( !target_body.empty() )
				m_pTargetBody = FindByName( model.GetBodies(), target_body ).get();
			else m_pTargetBody = nullptr;

            // initialize range
            range.min = Real( props.GetReal( "pos_min", 0.0 ) );
            range.max = Real( props.GetReal( "pos_max", 0.0 ) );
            velocity_range.min = Real( props.GetReal( "vel_min", 0.0 ) );
            velocity_range.max = Real( props.GetReal( "vel_max", 0.0 ) );

            point = Vec3(px, py, pz);
}

		sim::Controller::UpdateResult PointYMeasure::UpdateAnalysis( const sim::Model& model, double timestamp )
		{
			SCONE_PROFILE_SCOPE;

			if ( squared_range_penalty > 0.0 || abs_range_penalty > 0.0)
			{
                double pos = m_pTargetBody->GetPosOfPointFixedOnBody(point)[1];
				double range_violation = range.GetRangeViolation( pos );
				double rps = squared_range_penalty * GetSquared( range_violation );
				double rpa = abs_range_penalty * std::abs( range_violation );
				penalty.AddSample( timestamp, rps + rpa );
			}

			if ( squared_velocity_range_penalty > 0 || abs_velocity_range_penalty > 0 )
			{
                double vel = m_pTargetBody->GetLinVelOfPointFixedOnBody(point)[1];
				double range_violation = velocity_range.GetRangeViolation( vel );
				double vrps = squared_velocity_range_penalty * GetSquared( range_violation );
				double vrpa = abs_velocity_range_penalty * std::abs( range_violation );
				penalty.AddSample( timestamp, vrps + vrpa );
			}

			return SuccessfulUpdate;
		}

		double PointYMeasure::GetResult( sim::Model& model )
		{
			return penalty.GetAverage();
		}

		scone::String PointYMeasure::GetClassSignature() const
		{
			return "";
		}
	}
}
