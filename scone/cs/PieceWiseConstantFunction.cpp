#include "stdafx.h"
#include "PieceWiseConstantFunction.h"

namespace scone
{
	PieceWiseConstantFunction::PieceWiseConstantFunction()
	{

	}

	PieceWiseConstantFunction::PieceWiseConstantFunction( const PropNode& props, opt::ParamSet& par )
	{

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
