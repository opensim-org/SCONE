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

			virtual size_t GetDofCount() = 0;
			virtual Real GetDof( size_t index = 0 ) = 0;
			virtual const String& GetDofName( size_t index = 0 ) = 0;
		};
	}
}
