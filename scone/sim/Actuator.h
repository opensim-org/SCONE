#pragma once

#include "sim.h"
#include "../core/Named.h"

namespace scone
{
	namespace sim
	{
		class SCONE_SIM_API Actuator : public virtual Named
		{
		public:
			Actuator();
			virtual ~Actuator();

			virtual void AddControlValue( double v ) { m_ActuatorControlValue += v; }
			virtual void ResetControlValue() { m_ActuatorControlValue = 0.0; }
			double GetControlValue() const { return m_ActuatorControlValue; }

		private:
			double m_ActuatorControlValue;
		};
	}
}
