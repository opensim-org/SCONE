#pragma once

#include "sim.h"
#include "..\core\Propertyable.h"

namespace scone
{
	namespace sim
	{
		class SIM_API Controller : virtual public Propertyable
		{
		public:
			Controller();
			virtual ~Controller();

			virtual bool RegisterModel( ModelSP model ) { return true; }
			virtual void UnregisterModel( ModelSP model ) { };
			virtual bool Update( ModelSP model, double timestamp ) = 0;

			virtual void ProcessPropNode( PropNode& props );

		};
	}
}
