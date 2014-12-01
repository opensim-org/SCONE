#pragma once

#include "../core/types.h"
#include "sim.h"
#include "../core/Vec3.h"

namespace scone
{
	namespace sim
	{
		class SIM_API Model
		{
		public:
			Model();
			virtual ~Model();

			virtual Vec3 GetComPos() = 0;
			virtual Vec3 GetComVel() = 0;

			virtual Real GetMass() = 0;

			virtual size_t GetLinkCount() = 0;
			virtual class Link& GetLink( size_t idx ) = 0;

			virtual size_t GetMuscleCount() = 0;
			virtual class Muscle& GetMuscle( size_t idx ) = 0;
		};
	}
}
