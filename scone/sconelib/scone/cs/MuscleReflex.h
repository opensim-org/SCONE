#pragma once

#include "Reflex.h"
#include "scone/sim/SensorDelayAdapter.h"
#include "scone/core/Range.h"

namespace scone
{
	namespace cs
	{
		class SCONE_API MuscleReflex : public Reflex
		{
		public:
			MuscleReflex( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area );
			virtual ~MuscleReflex();

			virtual void ComputeControls( double timestamp ) override;

			// Reflex parameters
			Real length_gain;
			Real length_ofs;
			bool length_allow_negative;

			Real velocity_gain;
			Real velocity_ofs;
			bool velocity_allow_negative;

			Real force_gain;
			Real force_ofs;
			bool force_allow_negative;

			Real u_constant;

		private:
			sim::SensorDelayAdapter* m_pForceSensor;
			sim::SensorDelayAdapter* m_pLengthSensor;
			sim::SensorDelayAdapter* m_pVelocitySensor;
		};
	}
}
