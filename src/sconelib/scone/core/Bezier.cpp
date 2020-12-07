/*
** Bezier.cpp
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights
*reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "Bezier.h"
#include "scone/core/Exception.h"
#include "scone/core/PropNode.h"
#include "xo/container/prop_node_tools.h"
#include <cmath>
#include <cstddef>
#include <iostream>
using std::pow;

namespace scone
{
	int binomial( int n, int i )
	{
		if ( i == 0 || i == n )
			return 1;
		return binomial( n - 1, i ) + binomial( n - 1, i - 1 );
	}

	double bernstein( int n, int i, double t )
	{
		return binomial( n, i ) * pow( t, i ) * pow( 1 - t, n - i );
	}

	Bezier::Bezier( const std::vector< Real >& controlPoints )
		: control_points( controlPoints.size( ) ),
		  m_ControlPoints( controlPoints )
	{
	}

	Bezier::Bezier( const PropNode& props, Params& par )
	{
		INIT_PROP_REQUIRED( props, control_points );
		SCONE_CHECK_RANGE( control_points, 2, 99 );
		m_ControlPoints.resize( control_points );

		// try initializing from distribution: control_point_y = 0.3~0.3<0,1>
		if ( const auto* control_point_y =
				 props.try_get_child( "control_point_y" ) )
			for ( size_t i = 0; i < control_points; i++ )
				m_ControlPoints[ i ] =
					par.get( stringf( "Y%d", i ), *control_point_y );
		// else initialize control points if they are defined
		else
			for ( size_t i = 0; i < control_points; i++ )
				m_ControlPoints[ i ] = par.try_get( stringf( "Y%d", i ), props,
													stringf( "Y%d", i ), 0.0 );
	}

	Bezier::~Bezier( ) {}

	Real Bezier::GetValue( Real x )
	{
		SCONE_ASSERT_MSG( x >= 0 && x <= 1,
						  "Bezier curve expects 0 <= x <= 1" );

		Real B = 0;
		size_t n = m_ControlPoints.size( ) - 1; // n: degree
		for ( int i = 0; i <= n; i++ )
			B += bernstein( n, i, x ) * m_ControlPoints[ i ];

		return B;
	}

	Real Bezier::GetDerivativeValue( Real x )
	{
		SCONE_ASSERT_MSG( x >= 0 && x <= 1,
						  "Bezier curve expects 0 <= x <= 1" );

		Real B = 0;
		size_t n = m_ControlPoints.size( ) - 1; // n: degree
		for ( int i = 0; i <= n - 1; i++ )
			B += bernstein( n - 1, i, x ) *
				 ( m_ControlPoints[ i + 1 ] - m_ControlPoints[ i ] );

		return n * B;
	}

	String Bezier::GetSignature( )
	{
		return stringf( "B%d", m_ControlPoints.size( ) );
	}

} // namespace scone
