#pragma once

#include "sim.h"
#include "../core/HasName.h"
#include "ChannelSensor.h"
#include "../core/StringMap.h"

namespace scone
{
	namespace sim
	{
		class SCONE_SIM_API Dof : public ChannelSensor
		{
		public:
			Dof();
			virtual ~Dof();

			virtual Real GetPos() const = 0;
			virtual Real GetVel() const = 0;
			virtual Real GetLimitForce() const = 0;

			static const Index DofPositionSensor = 0;
			static const Index DofVelocitySensor = 1;

			virtual const StringIndexMap& GetSensorNames() const override;
			virtual Real GetSensorValue( Index idx ) const override;

		private:
			static StringMap< Index > m_SensorNames;
		};
	}
}
