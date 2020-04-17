/*
** MorphedOscillator.cpp
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "MorphedOscillator.h"
#include "scone/core/Exception.h"
#include "scone/core/Factories.h"
#include "scone/core/Function.h"

namespace scone
{
	MorphedOscillator::MorphedOscillator( const PropNode& props, Params& par, Model& model ) :
		StateComponent(props, par, model), m_synchronized(false)
	{
		// initialize parameters
		INIT_PAR( props, par, omega, 1 );
		INIT_PAR( props, par, gamma, 100.0 );
		INIT_PAR( props, par, K, 0.0 );
		INIT_PAR( props, par, theta0, 0.0 );
		INIT_PAR( props, par, x0, 0.0 );
		INIT_PAR( props, par, synchronization_state, GaitStateController::UnknownState );
		INIT_PAR( props, par, side, Side::NoSide );

		SCONE_THROW_IF(synchronization_state < -1 || synchronization_state > 4,
					   "-1 <= synchronization_state <= 4");
		SCONE_THROW_IF(side < -1 || side > 1, "-1 <= side <= 1");

		// get shape function
		auto fp = FindFactoryProps( GetFunctionFactory(), props, "Function" );
		m_shapeFunctio = CreateFunction( fp, par );
	}

	std::vector< Real > MorphedOscillator::GetInitialConditions() const
	{
		return std::vector< Real >{theta0, x0};
	}

	std::vector< Real > MorphedOscillator::CalcStateDerivatives( Real t, std::vector< Real > x0 ) const
	{
		auto theta = x0[0];
		auto x = x0[1];
		auto dtheta = omega;
		auto dx = gamma * (m_shapeFunctio->GetValue(theta) - x) +
			m_shapeFunctio->GetDerivativeValue(theta) * omega + K;
		return std::vector< Real >{dtheta, dx};
	}

	bool MorphedOscillator::HasDiscreteEvent() const
	{
		return true;
	}

	int MorphedOscillator::TriggeredOnSign() const
	{
		return 0;
	}

	Real MorphedOscillator::CheckForEvent( Real t, std::vector< Real > x ) const
	{
		// Check if oscillator should be synchronized with the
		// environment through GaitStateController. This is valid only
		// for gait.
		auto gaitController = dynamic_cast<GaitStateController*>(m_model.GetController());
		if (gaitController != nullptr) // check if GaitStateController exists
		{
			const auto& legStates = gaitController->GetLegStates();
			for (const auto& legState : legStates) // iterate over each leg to find the side
			{
				if (legState.get()->leg.GetSide() == side)
				{
					if (!m_synchronized && legState.get()->state == synchronization_state)
						return 0; // trigger an event until it is handled (EventHandler)
					else if (m_synchronized && legState.get()->state != synchronization_state)
						m_synchronized = false; // changed this when we are out of sync state

					break;
				}
			}
		}

		// Otherwise synchronize based on periodicity
		return sin(x[0]);
	}

	std::vector< Real > MorphedOscillator::EventHandler( Real t, std::vector< Real > x ) const
	{
		m_synchronized = true;	// unlatch the event
		auto xNew = x;
		xNew[0] = theta0;
		return xNew;
	}
}
