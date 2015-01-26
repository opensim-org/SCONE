#include "stdafx.h"
#include "PieceWiseLinearFunction.h"

namespace scone
{
	PieceWiseLinearFunction::PieceWiseLinearFunction( bool flat_extrapolation ) :
	m_FlatExtrapolation( flat_extrapolation )
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
}
