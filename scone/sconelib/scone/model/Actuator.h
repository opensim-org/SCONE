#pragma once

#include "scone/core/HasName.h"
#include "scone/core/HasData.h"

namespace scone
{
	class SCONE_API Actuator : public virtual HasName, public HasData
	{
	public:
		Actuator();
		virtual ~Actuator();

		virtual void AddInput( double v ) { m_ActuatorControlValue += v; }
		virtual void ClearInput() { m_ActuatorControlValue = 0.0; }
		virtual double GetInput() const { return m_ActuatorControlValue; }
		virtual void StoreData( Storage< Real >::Frame& frame ) override;

	protected:
		double m_ActuatorControlValue;
	};
}
