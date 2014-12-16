#pragma once

#include "sim.h"

namespace scone
{
	namespace sim
	{
		class SCONE_SIM_API Actuator
		{
		public:
			Actuator();
			virtual ~Actuator();

			virtual void AddControlValue( double v ) = 0;
		};
	}
}
