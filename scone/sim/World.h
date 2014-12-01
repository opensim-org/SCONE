#pragma once

#include "sim.h"
#include "Model.h"
#include <memory>

namespace scone
{
	namespace sim
	{
		class SIM_API World
		{
		public:
			World();;
			virtual ~World();;

			virtual std::unique_ptr< Model > CreateModel( String& filename ) = 0;
		};
	}
}
