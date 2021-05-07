/*
** DofLimitMeasure.cpp
**
** Copyright (C) 2013-2021 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "DofLimitMeasure.h"
#include "scone/model/Model.h"
#include "scone/core/profiler_config.h"
#include "scone/core/math.h"
#include "scone/core/string_tools.h"

namespace scone
{
	DofLimitMeasure::DofLimitMeasure( const PropNode& props, Params& par, const Model& model, const Location& loc ) :
	Measure( props, par, model, loc )
	{
		if ( const PropNode* lp = props.try_get_child( "Limits" ) )
		{
			for ( auto it = lp->begin(); it != lp->end(); ++it )
				m_Limits.push_back( Limit( it->second, model ) );
		}

		// see if we have a limit defined internally
		if ( props.try_get_child( "dof" ) )
		{
			m_Limits.push_back( Limit( props, model ) );
			if ( name.empty() )
				name = m_Limits.back().dof.GetName();
		}
	}

	DofLimitMeasure::Limit::Limit( const PropNode& props, const Model& model ) :
	dof( *FindByName( model.GetDofs(), props.get< String >( "dof" ) ) ),
	parent( props.has_key( "dof_parent" ) ? &*FindByName( model.GetDofs(), props.get< String >( "dof_parent" ) ) : nullptr ),
	penalty( Statistic<>::LinearInterpolation )
	{
		range.min = Degree( props.get< Real >( "min_deg", 0.0 ) );
		range.max = Degree( props.get< Real >( "max_deg", 0.0 ) );
		velocity_range.min = Degree( props.get< Real >( "min_deg_s", 0.0 ) );
		velocity_range.max = Degree( props.get< Real >( "max_deg_s", 0.0 ) );
		INIT_PROP( props, squared_range_penalty, 0 );
		INIT_PROP( props, abs_range_penalty, 0 );
		INIT_PROP( props, squared_velocity_range_penalty, 0 );
		INIT_PROP( props, abs_velocity_range_penalty, 0 );
		INIT_PROP( props, squared_force_penalty, 0 );
		INIT_PROP( props, abs_force_penalty, 0 );
	}

	bool DofLimitMeasure::UpdateMeasure( const Model& model, double timestamp )
	{
		SCONE_PROFILE_FUNCTION( model.GetProfiler() );

		for ( Limit& l : m_Limits )
		{
			if ( l.squared_range_penalty != 0 || l.abs_range_penalty != 0 )
			{
				auto value = Radian( l.dof.GetPos() + ( l.parent ? l.parent->GetPos() : 0 ) );
				double range_violation = l.range.GetRangeViolation( Degree( value ) ).value;
				double rps = l.squared_range_penalty * GetSquared( range_violation );
				double rpa = l.abs_range_penalty * std::abs( range_violation );
				l.penalty.AddSample( timestamp, rps + rpa );
			}

			if ( l.squared_velocity_range_penalty != 0 || l.abs_velocity_range_penalty != 0 )
			{
				auto value = Radian( l.dof.GetVel() + ( l.parent ? l.parent->GetVel() : 0 ) );
				double range_violation = l.velocity_range.GetRangeViolation( Degree( value ) ).value;
				double vrps = l.squared_velocity_range_penalty * GetSquared( range_violation );
				double vrpa = l.abs_velocity_range_penalty * std::abs( range_violation );
				l.penalty.AddSample( timestamp, vrps + vrpa );
			}

			if ( l.squared_force_penalty != 0 || l.abs_force_penalty != 0 )
			{
				double lf = l.dof.GetLimitMoment() + ( l.parent ? l.parent->GetLimitMoment() : 0 );
				double fps = l.squared_force_penalty * GetSquared( lf );
				double fpa = l.abs_force_penalty * abs( lf );
				l.penalty.AddSample( timestamp, fps + fpa );
			}
		}

		return false;
	}

	double DofLimitMeasure::ComputeResult( const Model& model )
	{
		double result = 0.0;
		for ( Limit& l : m_Limits )
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

	void DofLimitMeasure::StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags ) const
	{
		for ( auto& l : m_Limits )
			frame[ l.dof.GetName() + ".limit_penalty" ] = l.penalty.GetLatest();
	}
}
