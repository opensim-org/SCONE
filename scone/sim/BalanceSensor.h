#pragma once

#include "sim.h"
#include "Dof.h"

namespace scone
{
	namespace sim
	{
		class SCONE_SIM_API BalanceSensor : public Sensor
		{
		public:
			BalanceSensor( Model& model );
			virtual ~BalanceSensor();

			static const Index PostureSensorIndex = 0;

			virtual const StringIndexMap& GetSensorNames() const override;
			virtual Real GetSensorValue( Index idx ) const override;
			virtual const String& GetName() const;

		private:
			static const StringMap< Index > m_SensorNames;
			Dof& m_BodyDof;
			Dof& m_RootDof;
		};
	}
}


