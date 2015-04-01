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

			// muscle parameters
			virtual Real GetMaxIsometricForce() override;
			virtual Real GetOptimalFiberLength() override;
			virtual Real GetTendonSlackLength() override;
			virtual Real GetMass() override;
			
			// current force / length / velocity
			virtual Real GetForce() override;
			virtual Real GetLength() override;
			virtual Real GetVelocity() override;
			virtual Real GetFiberForce() override;
			virtual Real GetFiberLength() override;
			virtual Real GetFiberVelocity() override;
			virtual Real GetTendonLength() override;

			virtual Real GetActivation() override;
			virtual Real GetExcitation() override;

			virtual std::vector< Vec3 > GetMusclePath() override;

			virtual void SetExcitation( Real u ) override;

			OpenSim::Muscle& GetOsMuscle() { return m_osMus; }

			virtual const String& GetName() const override;

		private:
			OpenSim::Muscle& m_osMus;
			Model_Simbody& m_Model;
		};
	}
}
