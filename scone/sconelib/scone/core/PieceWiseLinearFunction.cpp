#include "PieceWiseLinearFunction.h"
#include "scone/core/propnode_tools.h"

namespace scone
{
	PieceWiseLinearFunction::PieceWiseLinearFunction( bool flatExtrapolation ) :
	flat_extrapolation( flatExtrapolation )
	{

	}

	PieceWiseLinearFunction::PieceWiseLinearFunction( const PropNode& props, ParamSet& par )
	{
		size_t control_points;
		INIT_PROPERTY( props, control_points, size_t( 0 ) );
		INIT_PROPERTY( props, flat_extrapolation, false );

		for ( size_t cpidx = 0; cpidx < control_points; ++cpidx )
		{
			Real xVal = 0.0;
			if ( cpidx > 0 )
			{
				double dt = par.get( stringf( "DT%d", cpidx - 1 ), props.get_child( "control_point_delta_time" ) );
				xVal = GetX( cpidx - 1 ) + dt;
			}
			Real yVal = par.get( stringf( "Y%d", cpidx ), props.get_child( "control_point_y" ) );
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
		return m_osFunc.getX( static_cast<int>( index ) );
	}

	scone::Real PieceWiseLinearFunction::GetY( size_t index )
	{
		return m_osFunc.getY( static_cast<int>( index ) );
	}
}
