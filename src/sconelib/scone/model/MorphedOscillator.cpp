/*
** MorphedOscillator.cpp
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "MorphedOscillator.h"
#include "scone/core/Factories.h"
#include "scone/core/Function.h"

namespace scone
{
	MorphedOscillator::MorphedOscillator( const PropNode& props, Params& par ) :
		StateComponent(props, par)
	{
		// initialize parameters
		INIT_PAR( props, par, omega, 1 );
		INIT_PAR( props, par, gamma, 100.0 );
		INIT_PAR( props, par, K, 0.0 );
		INIT_PAR( props, par, theta0, 0.0 );
		INIT_PAR( props, par, x0, 0.0 );

		// get shape function
		auto fp = FindFactoryProps( GetFunctionFactory(), props, "Function" );
		m_shapeFunctio = CreateFunction( fp, par );
	}

	std::vector< Real > MorphedOscillator::getInitialCondition() const
	{
		return std::vector< Real >{theta0, x0};
	}

	std::vector< Real > MorphedOscillator::calcStateDerivatives( Real t, std::vector< Real > x0 ) const
	{
		auto theta = x0[0];
		auto x = x0[1];
		auto dtheta = omega;
		auto dx = gamma * (m_shapeFunctio->GetValue(theta) - x) +
			m_shapeFunctio->GetDerivativeValue(theta) * omega + K;
		return std::vector< Real >{dtheta, dx};
	}
}
