#include "PieceWiseConstantFunction.h"
#include "scone/core/InitFromPropNode.h"

namespace scone
{
	PieceWiseConstantFunction::PieceWiseConstantFunction()
	{

	}

	PieceWiseConstantFunction::PieceWiseConstantFunction( const PropNode& props, opt::ParamSet& par )
	{
		size_t control_points;
		INIT_PROPERTY( props, control_points, size_t( 0 ) );

		for ( size_t cpidx = 0; cpidx < control_points; ++cpidx )
		{
			Real xVal = 0.0;
			if ( cpidx > 0 )
			{
				double dt = par.Get( stringf( "DT%d", cpidx - 1 ), props, "control_point_delta_time" );
				xVal = GetX( cpidx - 1 ) + dt;
			}
			Real yVal = par.Get( stringf( "Y%d", cpidx ), props, "control_point_y" );
			AddPoint( xVal, yVal );
		}
	}

	PieceWiseConstantFunction::~PieceWiseConstantFunction()
	{

	}

	scone::Real PieceWiseConstantFunction::GetValue( Real x )
	{
		SimTK::Vector xval( 1 );
		xval[ 0 ] = x;
		return m_osFunc.calcValue( xval );
	}

	void PieceWiseConstantFunction::AddPoint( Real x, Real y )
	{
		m_osFunc.addPoint( x, y );
	}

	scone::Real PieceWiseConstantFunction::GetX( size_t index )
	{
		return m_osFunc.getX( index );
	}

	scone::Real PieceWiseConstantFunction::GetY( size_t index )
	{
		return m_osFunc.getY( index );
	}

}
