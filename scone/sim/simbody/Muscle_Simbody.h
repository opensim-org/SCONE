#pragma once

#include "sim_simbody.h"
#include "../Muscle.h"

#include <vector>

namespace OpenSim
{
	class Muscle;
}

namespace scone
{
	namespace sim
	{
		class SCONE_SIM_SIMBODY_API Muscle_Simbody : public Muscle
		{
		public:
			Muscle_Simbody( OpenSim::Muscle& mus );
			virtual ~Muscle_Simbody();

			virtual Real GetLength() override;
			virtual Real GetFiberLength() override;
			virtual Real GetTendonLength() override;
			virtual Real GetMaxIsometricForce() override;
			virtual std::vector< Vec3 > GetMusclePath() override;

			virtual Real GetActivation() override;
			virtual Real GetExcitation() override;
			virtual void SetExcitation( Real u ) override;

			OpenSim::Muscle& GetOSMuscle() { return m_osMus; }

		private:
			OpenSim::Muscle& m_osMus;
		};
	}
}
