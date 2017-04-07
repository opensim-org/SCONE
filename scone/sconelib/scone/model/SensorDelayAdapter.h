#pragma once

#include "Sensor.h"

namespace scone
{
	class SCONE_API SensorDelayAdapter : public Sensor
	{
	public:
		SensorDelayAdapter( Model& model, Sensor& source, TimeInSeconds default_delay );
		virtual ~SensorDelayAdapter();

		virtual Real GetValue() const override;
		virtual Real GetValue( Index idx ) const override;
		virtual String GetName() const override;

		Real GetValue( Real delay ) const;
		Real GetValue( Index idx, Real delay ) const;

		void UpdateStorage();
		Sensor& GetInputSensor() { return m_InputSensor; }

		virtual Count GetChannelCount() override;

	private:
		Model& m_Model;
		Sensor& m_InputSensor;
		TimeInSeconds m_Delay;
		Index m_StorageIdx;
	};
}
