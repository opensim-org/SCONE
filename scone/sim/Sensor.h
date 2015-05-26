#pragma once

#include "sim.h"
#include "../core/HasName.h"
#include "../core/StringMap.h"

namespace scone
{
	namespace sim
	{
		class SCONE_SIM_API Sensor : public virtual HasName
		{
		public:
			Sensor();
			virtual ~Sensor();

			size_t GetSensorCount() const;
			const String& GetSensorName( Index idx ) const;
			Index GetSensorIndex( const String& name ) const;

			virtual const StringIndexMap& GetSensorNames() const = 0;
			virtual Real GetSensorValue( Index idx ) const = 0;
		};
	}
}
