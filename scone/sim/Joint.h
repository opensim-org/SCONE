#pragma once

#include "sim.h"
#include "../core/Named.h"

namespace scone
{
	namespace sim
	{
		class SCONE_SIM_API Joint : public Named
		{
		public:
			Joint();
			virtual ~Joint();
		};
	}
}
