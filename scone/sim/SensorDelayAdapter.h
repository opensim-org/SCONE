#pragma once

#include "sim.h"
#include "Sensor.h"

namespace scone
{
	namespace sim
	{
		class SCONE_SIM_API SensorDelayAdapter : public Sensor
		{
		public:
			SensorDelayAdapter( Model& model, Sensor& source, TimeInSeconds default_delay );
			virtual ~SensorDelayAdapter();

			virtual Real GetValue() const override;
			virtual String GetName() const override;

			Real GetValue( Real delay ) const;
			Real* GetValueArray( Real delay ) const;

			void UpdateStorage();
			Sensor& GetInputSensor() { return m_InputSensor; }

		private:
			Model& m_Model;
			Sensor& m_InputSensor;
			TimeInSeconds m_Delay;
			Index m_StorageIdx;
		};
	}
}
