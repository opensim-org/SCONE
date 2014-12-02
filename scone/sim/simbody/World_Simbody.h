#pragma once

#include "simbody.h"
#include "..\World.h"

#include <memory>

namespace scone
{
	namespace sim
	{
		class SIM_SIMBODY_API World_Simbody : public World
		{
		public:
			World_Simbody() { };
			virtual ~World_Simbody() { };

			virtual std::unique_ptr< Model > CreateModel( const String& filename ) override;
		};
	}
}
