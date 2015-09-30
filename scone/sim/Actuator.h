#pragma once

#include "sim.h"
#include "../core/HasName.h"
#include "../core/HasData.h"

namespace scone
{
	namespace sim
	{
		class SCONE_SIM_API Actuator : public virtual HasName, public HasData
		{
		public:
			Actuator();
			virtual ~Actuator();

			virtual void AddControlValue( double v ) { m_ActuatorControlValue += v; }
			virtual void ResetControlValue() { m_ActuatorControlValue = 0.0; }
			double GetControlValue() const { return m_ActuatorControlValue; }
			virtual void StoreData( Storage< Real >::Frame& frame ) override;

		private:
			double m_ActuatorControlValue;
		};
	}
}
