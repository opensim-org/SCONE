/*
** PieceWiseConstantFunction.cpp
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "PieceWiseConstantFunction.h"
#include "scone/core/string_tools.h"
#include "xo/numerical/piecewise_constant_function.h"
#include "Exception.h"

namespace scone
{
	struct PieceWiseConstantFunction::Impl {
		xo::piecewise_constant_function< double > m_osFunc;
	};

	PieceWiseConstantFunction::PieceWiseConstantFunction( const PropNode& props, Params& par ) :
		control_point_y( props.get_child( "control_point_y" ) ),
		control_point_dt( props.get_child( "control_point_dt" ) ),
		m_pImpl( new Impl )
	{
		INIT_PROP_REQUIRED( props, control_points );
		SCONE_CHECK_RANGE( control_points, 1, 99 );

		for ( index_t cpidx = 0; cpidx < control_points; ++cpidx )
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

	PieceWiseConstantFunction::~PieceWiseConstantFunction()
	{}

	scone::Real PieceWiseConstantFunction::GetValue( Real x )
	{
		return m_pImpl->m_osFunc( x );
	}

	String PieceWiseConstantFunction::GetSignature()
	{
		return stringf( "C%d", m_pImpl->m_osFunc.size() );
	}
}
