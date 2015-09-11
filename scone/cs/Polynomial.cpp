#include "stdafx.h"
#include "Polynomial.h"
#include "../core/InitFromPropNode.h"

namespace scone
{

	Polynomial::Polynomial( size_t degree )
	{
		m_Coeffs.resize( degree + 1 );
	}

	Polynomial::Polynomial( const PropNode& props, opt::ParamSet& par )
	{
		size_t degree;
		INIT_PROPERTY( props, degree, 0u );
		m_Coeffs.resize( degree + 1 );
		for ( size_t i = 0; i < m_Coeffs.size(); ++i )
			SetCoefficient( i, par.Get( GetStringF( "C%d", i ), props, GetStringF( "coefficient%d", i ) ) );
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
