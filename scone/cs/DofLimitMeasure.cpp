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
		dof( FindNamed( model.GetDofs(), props.GetStr( "dof" ) ) ),
		penalty( Statistic<>::LinearInterpolation )
		{
			range.min = props.GetReal( "min_deg", 0.0 );
			range.max = props.GetReal( "max_deg", 0.0 );
			INIT_PROPERTY( props, squared_range_penalty, 0.0 );
			INIT_PROPERTY( props, squared_force_penalty, 0.0 );
		}

		sim::Controller::UpdateResult DofLimitMeasure::UpdateAnalysis( sim::Model& model, double timestamp )
		{
			SCONE_PROFILE_SCOPE;

			BOOST_FOREACH( Limit& l, m_Limits )
			{
				if ( l.squared_range_penalty > 0.0 )
				{
					double rp = l.squared_range_penalty * GetSquared( l.range.GetRangeViolation( RadToDeg( l.dof.GetPos() ) ) );
					l.penalty.AddSample( timestamp, rp );
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
			BOOST_FOREACH( Limit& l, m_Limits )
			{
				result += l.penalty.GetAverage();
				m_Report.Set( l.dof.GetName(), GetStringF( "%g", l.penalty.GetAverage() ) );
			}

			return result;
		}

		scone::String DofLimitMeasure::GetMainSignature()
		{
			return "DL";
		}

		scone::PropNode DofLimitMeasure::GetReport()
		{
			return m_Report;
		}
	}
}
