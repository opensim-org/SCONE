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

			virtual bool RegisterModel( class Model& model ) = 0;
			virtual void UnregisterModel( class Model& model ) = 0;
			virtual bool Update( class Model& model, double timestamp ) = 0;
		};
	}
}
