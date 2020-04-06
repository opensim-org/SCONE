/*
** MorphedOscillator.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "StateComponent.h"
#include "scone/optimization/Params.h"
#include "scone/core/PropNode.h"

namespace scone
{
	/// TODO
	class SCONE_API MorphedOscillator : public StateComponent
	{
	public:
		MorphedOscillator( const PropNode& props, Params& par );

		///
		Real omega;
		///
		Real gamma;
		///
		Real K;
		///
		Real theta0;
		///
		Real x0;

		// The function that is to be learned by the morphed
		// oscillator. It can be any function supported in SCONE.
		FunctionUP m_shapeFunctio;

		std::vector< Real > getInitialCondition() const override;
		std::vector< Real > calcStateDerivatives( Real t, std::vector< Real > x0 ) const override;
	};
}
