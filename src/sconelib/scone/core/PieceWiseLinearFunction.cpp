/*
** PieceWiseLinearFunction.cpp
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "PieceWiseLinearFunction.h"
#include "scone/core/string_tools.h"
#include "xo/numerical/piecewise_linear_function.h"

namespace scone
{
	struct PieceWiseLinearFunction::Impl {
		xo::piecewise_linear_function< double > m_osFunc;
	};

	PieceWiseLinearFunction::PieceWiseLinearFunction( const PropNode& props, Params& par ) :
		m_pImpl( new Impl ),
		control_point_y( props.get_child( "control_point_y" ) ),
		control_point_dt( props.get_child( "control_point_dt" ) )
	{
		INIT_PROP( props, control_points, size_t( 0 ) );
		INIT_PROP( props, flat_extrapolation, false );

		for ( int cpidx = 0; cpidx < control_points; ++cpidx )
		{
			Real xVal = 0.0;
			if ( cpidx > 0 )
			{
				double dt = par.get( stringf( "DT%d", cpidx - 1 ), control_point_dt );
				xo_assert_msg( dt > 0.0, "control_point_dt must be > 0" );
				xVal = m_pImpl->m_osFunc.point( cpidx - 1 ).first + dt;
			}
			Real yVal = par.get( stringf( "Y%d", cpidx ), control_point_y );
			m_pImpl->m_osFunc.insert_point( xVal, yVal );
		}
	}

	PieceWiseLinearFunction::~PieceWiseLinearFunction()
	{
	}

	scone::Real PieceWiseLinearFunction::GetValue( Real x )
	{
		if ( flat_extrapolation )
			x = xo::min( x, m_pImpl->m_osFunc.data().back().first );

		return m_pImpl->m_osFunc( x );
	}

	String PieceWiseLinearFunction::GetSignature()
	{
		return stringf( "L%d", m_pImpl->m_osFunc.size() );
	}
}
