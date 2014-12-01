#pragma once

#include "../core/types.h"
#include "../core/Vec3.h"
#include "sim.h"
#include <vector>

namespace scone
{
	namespace sim
	{
		class SIM_API Muscle
		{
		public:
			Muscle();
			virtual ~Muscle();

			virtual Real GetLength() = 0;
			virtual Real GetFiberLength() = 0;
			virtual Real GetTendonLength() = 0;
			virtual Real GetMaxIsometricForce() = 0;
			virtual std::vector< Vec3 > GetMusclePath() = 0;

			virtual Real GetActivation() = 0;
			virtual Real GetExcitation() = 0;
			virtual void SetExcitation( Real u ) = 0;
		};
	}
}
