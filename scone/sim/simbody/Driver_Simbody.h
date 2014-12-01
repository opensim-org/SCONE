#pragma once

#include "simbody.h"
#include "..\Driver.h"

#include <memory>

namespace scone
{
	namespace sim
	{
		class SIM_SIMBODY_API Driver_Simbody : public Driver
		{
		public:
			Driver_Simbody() { };
			virtual ~Driver_Simbody() { };

			virtual std::unique_ptr< World > CreateWorld() override;
		};
	}
}
