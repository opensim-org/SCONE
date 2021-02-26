/*
** Polynomial.cpp
**
** Copyright (C) 2013-2021 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "Polynomial.h"

namespace scone
{
	Polynomial::Polynomial( size_t degree )
	{
		m_Coeffs.resize( degree + 1 );
	}

	Polynomial::Polynomial( const PropNode& props, Params& par )
	{
		size_t degree = props.get_any< size_t >( { "order", "degree" } );
		m_Coeffs.resize( degree + 1 );
		for ( size_t i = 0; i < m_Coeffs.size(); ++i )
			SetCoefficient( i, par.get( stringf( "C%d", i ), props.get_child( stringf( "coefficient%d", i ) ) ) );
	}

	Polynomial::~Polynomial()
	{
	}

	Real Polynomial::GetValue( Real x )
	{
		Real r = 0;
		Real c = 1;

		for ( auto iter = m_Coeffs.begin(); iter != m_Coeffs.end(); ++iter )
		{
			r += *iter * c;
			c *= x;
		}

		return r;
	}

	Real Polynomial::GetDerivativeValue( Real x )
	{
		Real r = 0, c = 1;
		for ( int i = 1; i < m_Coeffs.size(); i++ )
		{
			r += m_Coeffs[i] * c;
			c *= (i + 1) * x / i;
		}

		return r;
	}

	void Polynomial::SetCoefficient( size_t idx, Real value )
	{
		m_Coeffs[ idx ] = value;
	}

	size_t Polynomial::GetCoefficientCount()
	{
		return m_Coeffs.size();
	}
}
