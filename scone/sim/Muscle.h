#pragma once

#include "sim.h"
#include "../core/core.h"

#include <vector>
#include "Actuator.h"
#include "ChannelSensor.h"
#include "../core/StringMap.h"
#include "Sensors.h"

namespace scone
{
	namespace sim
	{
		class SCONE_SIM_API Muscle : public Actuator //, public ChannelSensor
		{
		public:
			Muscle();
			virtual ~Muscle();

			virtual Real GetMaxIsometricForce() const = 0;
			virtual Real GetOptimalFiberLength() const = 0;
			virtual Real GetTendonSlackLength() const = 0;
			virtual Real GetMass() const = 0;

			virtual Real GetForce() const = 0;
			virtual Real GetNormalizedForce() const = 0;

			virtual Real GetLength() const = 0;
			virtual Real GetVelocity() const = 0;

			virtual Real GetFiberForce() const = 0;
			virtual Real GetNormalizedFiberForce() const = 0;

			virtual Real GetFiberLength() const = 0;
			virtual Real GetNormalizedFiberLength() const = 0;

			virtual Real GetFiberVelocity() const = 0;
			virtual Real GetNormalizedFiberVelocity() const = 0;

			virtual Real GetTendonLength() const = 0;
			virtual std::vector< Vec3 > GetMusclePath() const = 0;

			virtual Real GetActivation() const = 0;
			virtual Real GetExcitation() const = 0;
			virtual void SetExcitation( Real u ) = 0;

			/// Inherited from Sensor
			//static const Index MuscleLengthSensor = 0;
			//static const Index MuscleVelocitySensor = 1;
			//static const Index MuscleForceSensor = 2;

			//virtual Real GetSensorValue( Index idx ) const override;
			//virtual const StringIndexMap& GetSensorNames() const override;
			
			// muscle sensors
			MuscleLengthSensor m_MuscleLengthSensor;
			MuscleForceSensor m_MuscleForceSensor;
			MuscleVelocitySensor m_MuscleVelocitySensor;
		};
	}
}
