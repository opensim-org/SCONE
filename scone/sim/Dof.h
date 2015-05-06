#pragma once

#include "sim.h"
#include "../core/HasName.h"
#include "Sensor.h"
#include "../core/StringMap.h"

namespace scone
{
	namespace sim
	{
		class SCONE_SIM_API Dof : public Sensor
		{
		public:
			Dof();
			virtual ~Dof();

			virtual Real GetPos() = 0;
			virtual Real GetVel() = 0;
			virtual Real GetLimitForce() = 0;

			static const Index DofSensorPosition = 0;
			static const Index DofSensorVelocity = 1;
			static const Index DofSensorCount = 2;
			static StringMap< Index > m_SensorNames;

			virtual size_t GetSensorCount() override;
			virtual const String& GetSensorName( Index idx ) override;
			virtual Real GetSensorValue( Index idx ) override;
		};
	}
}
