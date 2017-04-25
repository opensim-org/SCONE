#pragma once

#include "MuscleReflex.h"
#include "scone/core/Angle.h"

namespace scone
{
	class ConditionalMuscleReflex : public MuscleReflex
	{
	public:
		ConditionalMuscleReflex( const PropNode& props, ParamSet& par, Model& model, const Locality& area );
		virtual ~ConditionalMuscleReflex();

		virtual void ComputeControls( double timestamp ) override;

	protected:
		SensorDelayAdapter* m_pConditionalDofPos;
		SensorDelayAdapter* m_pConditionalDofVel;
		Range< Degree > m_ConditionalPosRange;
	};
}
