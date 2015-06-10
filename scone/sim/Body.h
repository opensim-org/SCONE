#pragma once

#include "sim.h"
#include "../core/Vec3.h"
#include "../core/Quat.h"
#include "Model.h"
#include "Sensor.h"

namespace scone
{
	namespace sim
	{
		class SCONE_SIM_API Body : public ChannelSensor
		{
		public:
			Body();
			virtual ~Body();

			virtual Vec3 GetPos() const = 0;
			virtual Quat GetOri() const = 0;

			virtual Vec3 GetLinVel() const = 0;
			virtual Vec3 GetAngVel() const = 0;

			virtual Vec3 GetContactForce() const = 0;
			virtual Vec3 GetContactTorque() const = 0;

			virtual const Model& GetModel() const = 0;

			static const Index BodySensorPosX = 0;
			static const Index BodySensorPosY = 0;
			static const Index BodySensorPosZ = 0;

			static const Index BodySensorOriX = 0;
			static const Index BodySensorOriY = 0;
			static const Index BodySensorOriZ = 0;

			static const Index BodySensorLinVelX = 0;
			static const Index BodySensorPosY = 0;
			static const Index BodySensorPosZ = 0;

			virtual Real GetSensorValue( Index idx ) const override;
			virtual const StringIndexMap& GetSensorNames() const override;
		};
	}
}
