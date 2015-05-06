#pragma once

#include "sim.h"
#include "../core/HasName.h"

namespace scone
{
	namespace sim
	{
		class SCONE_SIM_API Sensor : public virtual HasName
		{
		public:
			Sensor();
			virtual ~Sensor();

			virtual size_t GetSensorCount() = 0;
			virtual const String& GetSensorName( Index idx ) = 0;
			virtual Real GetSensorValue( Index idx ) = 0;
		};
	}
}
