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
		class Model_Simbody;

		class SCONE_SIM_SIMBODY_API Muscle_Simbody : public Muscle
		{
		public:
			Muscle_Simbody( Model_Simbody& model, OpenSim::Muscle& mus );
			virtual ~Muscle_Simbody();

			virtual Real GetForce() override;
			virtual Real GetLength() override;
			virtual Real GetFiberLength() override;
			virtual Real GetTendonLength() override;
			virtual Real GetMaxIsometricForce() override;
			virtual std::vector< Vec3 > GetMusclePath() override;

			virtual Real GetActivation() override;
			virtual Real GetExcitation() override;
			virtual void SetExcitation( Real u ) override;

			OpenSim::Muscle& GetOsMuscle() { return m_osMus; }

			virtual const String& GetName() const override;

		private:
			OpenSim::Muscle& m_osMus;
			Model_Simbody& m_Model;
		};
	}
}
