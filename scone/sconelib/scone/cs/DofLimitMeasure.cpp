#include "DofLimitMeasure.h"
#include "scone/sim/Model.h"
#include "scone/core/Profiler.h"
#include "scone/core/math.h"

namespace scone
{
	namespace cs
	{
		DofLimitMeasure::DofLimitMeasure( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area ) :
		Measure( props, par, model, area )
		{
			if ( const PropNode* lp = props.try_get( "Limits" ) )
			{
				for ( auto it = lp->begin(); it != lp->end(); ++it )
					m_Limits.push_back( Limit( it->second, model ) );
			}

			// see if we have a limit defined internally
			if ( props.try_get( "dof" ) )
				m_Limits.push_back( Limit( props, model ) );
		}

		DofLimitMeasure::~DofLimitMeasure() {}

		DofLimitMeasure::Limit::Limit( const PropNode& props, sim::Model& model ) :
		dof( *FindByName( model.GetDofs(), props.get< String >( "dof" ) ) ),
		penalty( Statistic<>::LinearInterpolation )
		{
			range.min = Degree( props.get< Real >( "min_deg", 0.0 ) );
			range.max = Degree( props.get< Real >( "max_deg", 0.0 ) );
			velocity_range.min = Degree( props.get< Real >( "min_deg_s", 0.0 ) );
			velocity_range.max = Degree( props.get< Real >( "max_deg_s", 0.0 ) );
			INIT_PROPERTY( props, squared_range_penalty, 0.0 );
			INIT_PROPERTY( props, abs_range_penalty, 0.0 );
			INIT_PROPERTY( props, squared_velocity_range_penalty, 0.0 );
			INIT_PROPERTY( props, abs_velocity_range_penalty, 0.0 );
			INIT_PROPERTY( props, squared_force_penalty, 0.0 );
		}

		sim::Controller::UpdateResult DofLimitMeasure::UpdateAnalysis( const sim::Model& model, double timestamp )
		{
			SCONE_PROFILE_SCOPE;

			if ( !IsActive( model, timestamp ) )
				return NoUpdate;

			for ( Limit& l: m_Limits )
			{
				if ( l.squared_range_penalty > 0.0 || l.abs_range_penalty > 0.0)
				{
					double range_violation = l.range.GetRangeViolation( Radian( l.dof.GetPos() ) ).value;
					double rps = l.squared_range_penalty * GetSquared( range_violation );
					double rpa = l.abs_range_penalty * std::abs( range_violation );
					l.penalty.AddSample( timestamp, rps + rpa );
				}

				if ( l.squared_velocity_range_penalty > 0 || l.abs_velocity_range_penalty > 0 )
				{
					double range_violation = l.velocity_range.GetRangeViolation( Radian( l.dof.GetVel() ) ).value;
					double vrps = l.squared_velocity_range_penalty * GetSquared( range_violation );
					double vrpa = l.abs_velocity_range_penalty * std::abs( range_violation );
					l.penalty.AddSample( timestamp, vrps + vrpa );
				}

				if ( l.squared_force_penalty > 0.0 )
				{
					double fp = l.squared_force_penalty * GetSquared( l.dof.GetLimitForce() );
					l.penalty.AddSample( timestamp, fp );
				}
			}

			return SuccessfulUpdate;
		}

		double DofLimitMeasure::GetResult( sim::Model& model )
		{
			double result = 0.0;
			for ( Limit& l: m_Limits )
			{
				result += l.penalty.GetAverage();
				if ( m_Limits.size() > 1 )
					GetReport().set( l.dof.GetName(), stringf( "%g", l.penalty.GetAverage() ) );
			}

			return result;
		}

		scone::String DofLimitMeasure::GetClassSignature() const
		{
			return "";
		}

		void DofLimitMeasure::StoreData( Storage< Real >::Frame& frame )
		{
			for ( Limit& l: m_Limits )
				frame[ l.dof.GetName() + ".limit_penalty" ] = l.penalty.GetLatest();
		}
	}
}
