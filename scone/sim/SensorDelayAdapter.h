#pragma once

#include "Sensor.h"
#include "../core/Delayer.h"
#include "../core/Storage.h"

namespace scone
{
	namespace sim
	{
		class Model;

		// TODO: make more efficient using templates?
		class SCONE_SIM_API SensorDelayAdapter : public Sensor
		{
		public:
			SensorDelayAdapter( Model& model, Storage< Real >& storage, Sensor& source, TimeInSeconds default_delay );
			virtual ~SensorDelayAdapter();

			virtual size_t GetSensorCount() override;
			virtual const String& GetSensorName( size_t idx ) override;
			virtual Real GetSensorValue( size_t idx ) override;
			Real GetSensorValue( size_t idx, TimeInSeconds delay );
			virtual const String& GetName() const override;
		
		private:
			friend Model;
			void UpdateStorage();

			Index m_ChannelOfs;
			TimeInSeconds m_Delay;
			Model& m_Model;
			Storage< Real >& m_Storage;
			Sensor& m_Source;
			int m_PreviousUpdateStep;
		};
	}
}
