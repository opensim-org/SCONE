#include "PointMeasure.h"
#include "scone/sim/Model.h"
#include "scone/core/Log.h"
#include "scone/core/Profiler.h"

namespace scone
{
	namespace cs
	{
		PointMeasure::PointMeasure( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area ) :
		Measure( props, par, model, area ),
		m_pTargetBody( nullptr )
		{
			INIT_PROPERTY( props, body, String("") );
			INIT_PROPERTY( props, offset, Vec3::zero() );
			INIT_PROPERTY_REQUIRED( props, axis );
			INIT_PROPERTY( props, squared_range_penalty, 0.0 );
			INIT_PROPERTY( props, abs_range_penalty, 0.0 );
			INIT_PROPERTY( props, squared_velocity_range_penalty, 0.0 );
			INIT_PROPERTY( props, abs_velocity_range_penalty, 0.0 );

			// find target body
			if ( !body.empty() )
				m_pTargetBody = FindByName( model.GetBodies(), body ).get();
			else m_pTargetBody = nullptr;

			// initialize range
			range.min = Real( props.GetReal( "pos_min", 0.0 ) );
			range.max = Real( props.GetReal( "pos_max", 0.0 ) );
			velocity_range.min = Real( props.GetReal( "vel_min", 0.0 ) );
			velocity_range.max = Real( props.GetReal( "vel_max", 0.0 ) );
}

		sim::Controller::UpdateResult PointMeasure::UpdateAnalysis( const sim::Model& model, double timestamp )
		{
			SCONE_PROFILE_SCOPE;

			if ( squared_range_penalty > 0.0 || abs_range_penalty > 0.0)
			{
				double pos = m_pTargetBody->GetPosOfPointFixedOnBody(offset)[axis_ind];
				double range_violation = range.GetRangeViolation( pos );
				double rps = squared_range_penalty * GetSquared( range_violation );
				double rpa = abs_range_penalty * std::abs( range_violation );
				penalty.AddSample( timestamp, rps + rpa );
			}

			if ( squared_velocity_range_penalty > 0 || abs_velocity_range_penalty > 0 )
			{
				double vel = m_pTargetBody->GetLinVelOfPointFixedOnBody(offset)[axis_ind];
				double range_violation = velocity_range.GetRangeViolation( vel );
				double vrps = squared_velocity_range_penalty * GetSquared( range_violation );
				double vrpa = abs_velocity_range_penalty * std::abs( range_violation );
				penalty.AddSample( timestamp, vrps + vrpa );
			}

			return SuccessfulUpdate;
		}

		double PointMeasure::GetResult( sim::Model& model )
		{
			return penalty.GetAverage();
		}

		scone::String PointMeasure::GetClassSignature() const
		{
			return "";
		}

		void PointMeasure::StoreData( Storage< Real >::Frame& frame )
		{
			frame[ m_pTargetBody->GetName() + ".point_penalty" ] = penalty.GetLatest();
		}
	}
}
