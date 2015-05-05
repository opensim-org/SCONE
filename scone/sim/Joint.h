#pragma once

#include "sim.h"
#include "../core/HasName.h"

namespace scone
{
	namespace sim
	{
		class SCONE_SIM_API Joint : public HasName
		{
		public:
			Joint();
			virtual ~Joint();

			virtual Vec3 GetPos() const = 0;
			virtual size_t GetDofCount() const = 0;
			virtual Real GetDof( size_t index = 0 ) const = 0;
			virtual const String& GetDofName( size_t index = 0 ) const = 0;
		};
	}
}
