#pragma once

#include "cs.h"
#include "Reflex.h"
#include "../sim/SensorDelayAdapter.h"
#include "../core/Range.h"

namespace scone
{
	namespace cs
	{
		class CS_API MuscleReflex : public Reflex
		{
		public:
			MuscleReflex( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area );
			virtual ~MuscleReflex();

			virtual void ComputeControls( double timestamp );

			// Reflex parameters
			Real length_gain;
			Real length_ofs;
			Real force_gain;
			Real velocity_gain;

		private:
			sim::SensorDelayAdapter* m_pConditionalDofPos;
			sim::SensorDelayAdapter* m_pConditionalDofVel;
			Range< Degree > m_ConditionalPosRange;

			sim::SensorDelayAdapter* m_pForceSensor;
			sim::SensorDelayAdapter* m_pLengthSensor;
			sim::SensorDelayAdapter* m_pVelocitySensor;
		};
	}
}
