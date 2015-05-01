#include "stdafx.h"
#include "PieceWiseLinearFunction.h"
#include "../core/InitFromPropNode.h"

namespace scone
{
	PieceWiseLinearFunction::PieceWiseLinearFunction( bool flatExtrapolation ) :
	flat_extrapolation( flatExtrapolation )
	{

	}

	PieceWiseLinearFunction::PieceWiseLinearFunction( const PropNode& props, opt::ParamSet& par )
	{
		size_t control_points;
		INIT_PROPERTY( props, control_points, 0u );
		INIT_PROPERTY( props, flat_extrapolation, false );

		for ( size_t cpidx = 0; cpidx < control_points; ++cpidx )
		{
			Real xVal = 0.0;
			if ( cpidx > 0 )
			{
				double dt = par.Get( GetStringF( "DT%d", cpidx - 1 ), props.GetChild( "control_point_delta_time" ) );
				xVal = GetX( cpidx - 1 ) + dt;
			}
			Real yVal = par.Get( GetStringF( "Y%d", cpidx ), props.GetChild( "control_point_y" ) );
			AddPoint( xVal, yVal );
		}
	}

	PieceWiseLinearFunction::~PieceWiseLinearFunction()
	{
	}

	scone::Real PieceWiseLinearFunction::GetValue( Real x )
	{
		SimTK::Vector xval( 1 );
		xval[ 0 ] = flat_extrapolation ? std::min( x, m_osFunc.getX( m_osFunc.getNumberOfPoints() - 1) ) : x;

		return m_osFunc.calcValue( xval );
	}

	void PieceWiseLinearFunction::AddPoint( Real x, Real y )
	{
		m_osFunc.addPoint( x, y );
	}

	scone::Real PieceWiseLinearFunction::GetX( size_t index )
	{
		return m_osFunc.getX( index );
	}

	scone::Real PieceWiseLinearFunction::GetY( size_t index )
	{
		return m_osFunc.getY( index );
	}
}
