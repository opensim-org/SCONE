#pragma once

#include "sim.h"
#include "../core/HasName.h"

namespace scone
{
	namespace sim
	{
		class SCONE_SIM_API Dof : public HasName
		{
		public:
			Dof();
			virtual ~Dof();

			virtual Real GetPos() = 0;
			virtual Real GetVel() = 0;
			virtual Real GetLimitForce() = 0;
		};
	}
}
