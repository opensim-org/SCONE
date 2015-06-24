#pragma once

#include "sim.h"
#include "../core/HasName.h"
#include "Sensors.h"

namespace scone
{
	namespace sim
	{
		class SCONE_SIM_API Dof : public HasName
		{
		public:
			Dof();
			virtual ~Dof();

			virtual Real GetPos() const = 0;
			virtual Real GetVel() const = 0;
			virtual Real GetLimitForce() const = 0;
		};
	}
}
