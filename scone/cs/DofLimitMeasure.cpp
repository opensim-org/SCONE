#include "stdafx.h"

#include "DofLimitMeasure.h"
#include "../sim/Model.h"
#include <boost/foreach.hpp>
#include "../core/Profiler.h"

namespace scone
{
	namespace cs
	{
		DofLimitMeasure::DofLimitMeasure( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area ) :
		Measure( props, par, model, area )
		{
			const PropNode& lp = props.GetChild( "Limits" );
			for ( auto it = lp.Begin(); it != lp.End(); ++it )
				m_Limits.push_back( Limit( *it->second, model ) );
		}

		DofLimitMeasure::~DofLimitMeasure()
		{
		}

		DofLimitMeasure::Limit::Limit( const PropNode& props, sim::Model& model ) :
		dof( *FindByName( model.GetDofs(), props.GetStr( "dof" ) ) ),
		penalty( Statistic<>::LinearInterpolation )
		{
			range.min = Degree( props.GetReal( "min_deg", 0.0 ) );
			range.max = Degree( props.GetReal( "max_deg", 0.0 ) );
			INIT_PROPERTY( props, squared_range_penalty, 0.0 );
			INIT_PROPERTY( props, squared_force_penalty, 0.0 );
			INIT_PROPERTY( props, squared_velocity_penalty, 0.0 );
		}

		sim::Controller::UpdateResult DofLimitMeasure::UpdateAnalysis( const sim::Model& model, double timestamp )
		{
			SCONE_PROFILE_SCOPE;

			BOOST_FOREACH( Limit& l, m_Limits )
			{
				if ( l.squared_range_penalty > 0.0 )
				{
					double rp = l.squared_range_penalty * GetSquared( l.range.GetRangeViolation( Radian( l.dof.GetPos() ) ) );
					l.penalty.AddSample( timestamp, rp );
				}

				if ( l.squared_force_penalty > 0.0 )
				{
					double fp = l.squared_force_penalty * GetSquared( l.dof.GetLimitForce() );
					l.penalty.AddSample( timestamp, fp );
				}

				if ( l.squared_velocity_penalty > 0.0 )
				{
					double vp = l.squared_velocity_penalty * GetSquared( l.dof.GetVel() );
					l.penalty.AddSample( timestamp, vp );
				}
			}

			return SuccessfulUpdate;
		}

		double DofLimitMeasure::GetResult( sim::Model& model )
		{
			double result = 0.0;
			BOOST_FOREACH( Limit& l, m_Limits )
			{
				result += l.penalty.GetAverage();
				m_Report.Set( l.dof.GetName(), GetStringF( "%g", l.penalty.GetAverage() ) );
			}

			return result;
		}

		scone::String DofLimitMeasure::GetClassSignature() const
		{
			return "";
		}

		void DofLimitMeasure::StoreData( Storage< Real >::Frame& frame )
		{
			BOOST_FOREACH( Limit& l, m_Limits )
				frame[ l.dof.GetName() + ".limit_penalty" ] = l.penalty.GetLatest();
		}

		scone::PropNode DofLimitMeasure::GetReport()
		{
			return m_Report;
		}
	}
}
