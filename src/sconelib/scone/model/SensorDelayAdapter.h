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
		virtual Real GetValue( index_t idx ) const override;
		virtual String GetName() const override;

		Real GetValue( Real delay ) const;
		Real GetValue( index_t idx, Real delay ) const;

		Real GetAverageValue( int delay_samples, int window_size ) const;

		void UpdateStorage();
		Sensor& GetInputSensor() { return m_InputSensor; }

		virtual size_t GetChannelCount() override;

	private:
		Model& m_Model;
		Sensor& m_InputSensor;
		TimeInSeconds m_Delay;
		index_t m_StorageIdx;
	};
}
