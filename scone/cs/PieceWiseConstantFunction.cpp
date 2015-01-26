#include "stdafx.h"
#include "PieceWiseConstantFunction.h"

namespace scone
{
	PieceWiseConstantFunction::PieceWiseConstantFunction()
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
}
