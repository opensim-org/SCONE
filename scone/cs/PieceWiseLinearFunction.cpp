#include "stdafx.h"
#include "PieceWiseLinearFunction.h"

namespace scone
{
	PieceWiseLinearFunction::PieceWiseLinearFunction( bool flat_extrapolation ) :
	m_FlatExtrapolation( flat_extrapolation )
	{

	}

	PieceWiseLinearFunction::PieceWiseLinearFunction( const PropNode& props, opt::ParamSet& par )
	{

	}

	PieceWiseLinearFunction::~PieceWiseLinearFunction()
	{

	}

	scone::Real PieceWiseLinearFunction::GetValue( Real x )
	{
		SimTK::Vector xval( 1 );
		xval[ 0 ] = m_FlatExtrapolation ? std::min( x, m_osFunc.getX( m_osFunc.getNumberOfPoints() - 1) ) : x;

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
