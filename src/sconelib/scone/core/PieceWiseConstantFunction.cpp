#include "PieceWiseConstantFunction.h"
#include "scone/core/propnode_tools.h"
#include "OpenSim/Common/PiecewiseConstantFunction.h"

namespace scone
{
	struct PieceWiseConstantFunction::Impl {
		OpenSim::PiecewiseConstantFunction m_osFunc;
	};

	PieceWiseConstantFunction::PieceWiseConstantFunction( const PropNode& props, Params& par ) :
	m_pImpl( new Impl )
	{
		size_t control_points;
		INIT_PROP( props, control_points, size_t( 0 ) );

		for ( int cpidx = 0; cpidx < control_points; ++cpidx )
		{
			Real xVal = 0.0;
			if ( cpidx > 0 )
			{
				double dt = par.get( stringf( "DT%d", cpidx - 1 ), props.get_child( "control_point_dt" ) );
				xVal = m_pImpl->m_osFunc.getX( cpidx - 1 ) + dt;
			}
			Real yVal = par.get( stringf( "Y%d", cpidx ), props.get_child( "control_point_y" ) );
			m_pImpl->m_osFunc.addPoint( xVal, yVal );
		}
	}

	PieceWiseConstantFunction::~PieceWiseConstantFunction()
	{}

	scone::Real PieceWiseConstantFunction::GetValue( Real x )
	{
		SimTK::Vector xval( 1 );
		xval[ 0 ] = x;
		return m_pImpl->m_osFunc.calcValue( xval );
	}

	String PieceWiseConstantFunction::GetSignature()
	{
		return stringf( "C%d", m_pImpl->m_osFunc.getSize() );
	}

}
