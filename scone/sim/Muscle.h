#pragma once

#include "sim.h"
#include "../core/core.h"

#include <vector>
#include "Actuator.h"

namespace scone
{
	namespace sim
	{
		class SCONE_SIM_API Muscle : public virtual Actuator
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
