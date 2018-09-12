#pragma once

#include "MuscleReflex.h"
#include "scone/core/Angle.h"

namespace scone
{
	/// Muscle reflex that is only active when a specific dof is within a specific range.
	class ConditionalMuscleReflex : public MuscleReflex
	{
	public:
		ConditionalMuscleReflex( const PropNode& props, Params& par, Model& model, const Locality& area );
		virtual ~ConditionalMuscleReflex();

		/// Name of the DoF used for the condition.
		Dof& dof;

		/// Maximum dof position [rad] for this reflex to be active.
		ParInfo pos_max;

		/// Minimum dof position [rad] for this reflex to be active.
		ParInfo pos_min;

		virtual void ComputeControls( double timestamp ) override;

	protected:
		SensorDelayAdapter* m_pConditionalDofPos;
		SensorDelayAdapter* m_pConditionalDofVel;
		Range< Degree > m_ConditionalPosRange;
	};
}
