#pragma once

#include "MuscleReflex.h"
#include "scone/core/Angle.h"

namespace scone
{
	namespace cs
	{
		class ConditionalMuscleReflex : public MuscleReflex
		{
		public:
			ConditionalMuscleReflex( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area );
			virtual ~ConditionalMuscleReflex();

			virtual void ComputeControls( double timestamp ) override;

		protected:
			sim::SensorDelayAdapter* m_pConditionalDofPos;
			sim::SensorDelayAdapter* m_pConditionalDofVel;
			Range< Degree > m_ConditionalPosRange;
		};
	}
}
