#pragma once

#include "Sensor.h"
#include "../core/Delayer.h"
#include "Model.h"

namespace scone
{
	namespace sim
	{
		// TODO: make more efficient using templates
		class SensorDelayAdapter : public Sensor
		{
		public:
			SensorDelayAdapter( Model& model, Sensor& source, double delay );
			virtual ~SensorDelayAdapter();

			virtual size_t GetSensorCount() override;
			virtual const String& GetSensorName( size_t idx ) override;
			virtual Real GetSensorValue( size_t idx ) override;
			virtual const String& GetName() const override;
		
		private:
			void UpdateFromSource();

			Model& m_Model;
			Sensor& m_Source;
			std::vector< Delayer< Real > > m_Delayers;
			int m_PreviousUpdateStep;
		};
	}
}
