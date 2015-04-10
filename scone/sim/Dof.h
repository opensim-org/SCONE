#pragma once

#include "sim.h"
#include "../core/Named.h"

namespace scone
{
	namespace sim
	{
		class SCONE_SIM_API Dof : public Named
		{
		public:
			Dof();
			virtual ~Dof();

			virtual Real GetPos() = 0;
			virtual Real GetVel() = 0;
		};
	}
}
