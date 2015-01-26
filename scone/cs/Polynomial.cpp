#include "stdafx.h"
#include "Polynomial.h"

namespace scone
{

	Polynomial::Polynomial( size_t degree )
	{
		m_Coeffs.resize( degree );
	}

	Polynomial::~Polynomial()
	{
	}

	scone::Real Polynomial::GetValue( Real x )
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

	void Polynomial::SetCoefficient( size_t idx, Real value )
	{
		m_Coeffs[ idx ] = value;
	}

	size_t Polynomial::GetCoefficientCount()
	{
		return m_Coeffs.size();
	}

}
