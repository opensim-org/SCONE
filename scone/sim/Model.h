#pragma once

#include "../core/core.h"
#include "sim.h"
#include "../core/Vec3.h"
#include "Controller.h"

namespace scone
{
	namespace sim
	{
		class SCONE_SIM_API Model
		{
		public:
			Model();
			virtual ~Model();

			virtual Vec3 GetComPos() = 0;
			virtual Vec3 GetComVel() = 0;
			virtual Real GetMass() = 0;

			std::vector< MuscleSP >& GetMuscles() { return m_Muscles; }
			std::vector< BodySP >& GetBodies() { return m_Bodies; }

			virtual void AddController( ControllerSP controller ) = 0;

		protected:
			std::vector< MuscleSP > m_Muscles;
			std::vector< BodySP > m_Bodies;
		};
	}
}
