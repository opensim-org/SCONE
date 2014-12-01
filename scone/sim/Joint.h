#pragma once
#include "sim.h"

namespace scone
{
	namespace sim
	{
		class SIM_API Joint
		{
		public:
			Joint();
			virtual ~Joint();
			
			virtual class Link& GetParentLink() = 0;
			virtual class Link& GetChildLink() = 0;

			virtual class Model& GetModel() = 0;
		
		};
	}
}
