#pragma once

#include "sim_simbody.h"
#include "../Muscle.h"

#include <vector>

namespace scone
{
	namespace sim
	{
		class SCONE_SIM_SIMBODY_API Muscle_Simbody : public Muscle
		{
		public:
			Muscle_Simbody() { };
			virtual ~Muscle_Simbody() { };

			virtual Real GetLength() override;
			virtual Real GetFiberLength() override;
			virtual Real GetTendonLength() override;
			virtual Real GetMaxIsometricForce() override;
			virtual std::vector< Vec3 > GetMusclePath() override;

			virtual Real GetActivation() override;
			virtual Real GetExcitation() override;
			virtual void SetExcitation( Real u ) override;
		};
	}
}
