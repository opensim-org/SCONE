/*
** MorphedOscillator.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "StateComponent.h"

namespace scone
{
	/// Implements morphed oscillator from Dzeladini et al. 2014
	/// (2.5.2. CPG-Oscillator model). This might change.
	class SCONE_API MorphedOscillator : public StateComponent
	{
	public:
		MorphedOscillator( const PropNode& props, Params& par );

		/// Oscillation frequency. Default 1.
		Real omega;
		/// Gamma constant. Default 100.
		Real gamma;
		/// Oscillator output's offset. Default 0.
		Real K;
		/// Oscillator phase's initial state. Default 0.
		Real theta0;
		/// Oscillator output's initial state. Default 0.
		Real x0;

		// The function that is to be learned by the morphed oscillator. It can
		// be any function supported in SCONE.
		FunctionUP m_shapeFunctio;

		std::vector< Real > GetInitialCondition() const override;
		std::vector< Real > CalcStateDerivatives( Real t, std::vector< Real > x0 ) const override;
		bool HasDiscreteEvent() const;
		int TriggeredOnSign() const;
		Real CheckForEvent( Real t, std::vector< Real > x ) const;
		std::vector< Real > EventHandler( Real t, std::vector< Real > x ) const;
	};
}
