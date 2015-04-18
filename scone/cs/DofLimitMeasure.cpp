#include "stdafx.h"

#include "DofLimitMeasure.h"
#include "../sim/Model.h"
#include <boost/foreach.hpp>

namespace scone
{
	namespace cs
	{
		DofLimitMeasure::DofLimitMeasure( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area ) :
		Measure( props, par, model, area ),
		m_Penalty( Statistic<>::LinearInterpolation )
		{
			const PropNode& lp = props.GetChild( "Limits" );
			for ( auto it = lp.Begin(); it != lp.End(); ++it )
				m_Limits.push_back( Limit( *it->second, model ) );
		}

		DofLimitMeasure::~DofLimitMeasure()
		{
		}

		DofLimitMeasure::Limit::Limit( const PropNode& props, sim::Model& model ) :
		dof( FindNamed( model.GetDofs(), props.GetStr( "dof" ) ) )
		{
			range.min = DegToRad( props.GetReal( "min_deg" ) );
			range.max = DegToRad( props.GetReal( "max_deg" ) );
			INIT_FROM_PROP( props, penalty, 1.0 );
		}

		void DofLimitMeasure::UpdateControls( sim::Model& model, double timestamp )
		{
			double penalty = 0.0;
			BOOST_FOREACH( Limit& l, m_Limits )
			{
				double violation = l.range.GetRangeViolation( l.dof.GetPos() );
				penalty += l.penalty * abs( violation );
			}

			m_Penalty.AddSample( penalty, timestamp );
		}

		double DofLimitMeasure::GetResult( sim::Model& model )
		{
			return m_Penalty.GetAverage();
		}

		scone::String DofLimitMeasure::GetSignature()
		{
			return "dl";
		}
	}
}
