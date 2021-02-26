/*
** ConditionalMuscleReflex.h
**
** Copyright (C) 2013-2021 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "MuscleReflex.h"
#include "scone/core/Angle.h"
#include "scone/core/Range.h"

namespace scone
{
	/// MuscleReflex that is only active when a specific DOF is within a specific range.
	class ConditionalMuscleReflex : public MuscleReflex
	{
	public:
		ConditionalMuscleReflex( const PropNode& props, Params& par, Model& model, const Location& loc );
		virtual ~ConditionalMuscleReflex();

		/// Name of the DoF used for the condition.
		Dof& dof;

		/// Maximum dof position [rad or m] for this reflex to be active; default = 1e12.
		Real pos_max;

		/// Minimum dof position [rad or m] for this reflex to be active; default = -1e12.
		Real pos_min;

		virtual void ComputeControls( double timestamp ) override;

	protected:
		SensorDelayAdapter* m_pConditionalDofPos;
		SensorDelayAdapter* m_pConditionalDofVel;
		Range< Real > m_ConditionalPosRange;
	};
}
