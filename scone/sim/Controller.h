#pragma once

#include "sim.h"

namespace scone
{
	namespace sim
	{
		class SIM_API Controller
		{
		public:
			Controller();
			virtual ~Controller();

			virtual bool RegisterModel( ModelSP model ) = 0;
			virtual void UnregisterModel( ModelSP model ) = 0;
			virtual bool Update( ModelSP model, double timestamp ) = 0;
		};
	}
}
