#pragma once

#include "MuscleReflex.h"
#include "scone/core/Angle.h"

namespace scone
{
	/// MuscleReflex that is only active when a specific DOF is within a specific range.
	class ConditionalMuscleReflex : public MuscleReflex
	{
	public:
		ConditionalMuscleReflex( const PropNode& props, Params& par, Model& model, const Locality& area );
		virtual ~ConditionalMuscleReflex();

		/// Name of the DoF used for the condition.
		Dof& dof;

		/// Maximum dof position [deg] for this reflex to be active; default = 180.
		Degree pos_max;

		/// Minimum dof position [deg] for this reflex to be active; default = -180.
		Degree pos_min;

		virtual void ComputeControls( double timestamp ) override;

	protected:
		SensorDelayAdapter* m_pConditionalDofPos;
		SensorDelayAdapter* m_pConditionalDofVel;
		Range< Degree > m_ConditionalPosRange;
	};
}
