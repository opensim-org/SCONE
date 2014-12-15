#pragma once
#include "sim.h"

namespace scone
{
	namespace sim
	{
		class SCONE_SIM_API Joint
		{
		public:
			Joint();
			virtual ~Joint();
			
			virtual class Body& GetParentLink() = 0;
			virtual class Body& GetChildLink() = 0;

			virtual class Model& GetModel() = 0;
		
		};
	}
}
