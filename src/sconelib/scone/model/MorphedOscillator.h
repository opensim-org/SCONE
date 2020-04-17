/*
** MorphedOscillator.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "StateComponent.h"
#include "scone/controllers/GaitStateController.h"

namespace scone
{

	/// Implements morphed oscillator from Dzeladini et al. 2014
	/// (2.5.2. CPG-Oscillator model). This might change.
	class SCONE_API MorphedOscillator : public StateComponent
	{
	public:
		MorphedOscillator( const PropNode& props, Params& par, Model& model );

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
		/// Gait state at which the oscillator is reset (synchronized
		/// with the environment) to initial conditions. Possible
		/// values: UnknownState = -1, EarlyStanceState = 0,
		/// LateStanceState = 1, LiftoffState = 2, SwingState = 3,
		/// LandingState = 4. default -1 (UnknownState).
		GaitStateController::GaitState synchronization_state;
		/// Trigger the synchronization when the corresponding leg is
		/// in the mentioned state. Possible values: LeftSide = -1,
		/// NoSide = 0, RightSide = 1. default 0 (NoSide).
		Side side;

		// The function that is to be learned by the morphed oscillator. It can
		// be any function supported in SCONE.
		FunctionUP m_shapeFunctio;

		std::vector< Real > GetInitialConditions() const override;
		std::vector< Real > CalcStateDerivatives( Real t, std::vector< Real > x0 ) const override;
		bool HasDiscreteEvent() const override;
		int TriggeredOnSign() const override;
		Real CheckForEvent( Real t, std::vector< Real > x ) const override;
		std::vector< Real > EventHandler( Real t, std::vector< Real > x ) const override;

	private:
		mutable bool m_synchronized;
	};
}
