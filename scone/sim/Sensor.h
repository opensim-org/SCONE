#pragma once

#include "sim.h"
#include "../core/Named.h"

namespace scone
{
	namespace sim
	{
		class SCONE_SIM_API Sensor : public virtual Named
		{
		public:
			Sensor();
			virtual ~Sensor();

			virtual size_t GetSensorCount() = 0;
			virtual const String& GetSensorName( size_t idx ) = 0;
			virtual Real GetSensorValue( size_t idx ) = 0;
		};
	}
}
