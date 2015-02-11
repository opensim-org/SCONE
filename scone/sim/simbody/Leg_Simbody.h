#pragma once

#include "sim_simbody.h"
#include "../Leg.h"

namespace scone
{
	namespace sim
	{
		class Model_Simbody;

		class SCONE_SIM_SIMBODY_API Leg_Simbody : public Leg
		{
		public:
			Leg_Simbody( Model_Simbody& model, const Link& upper, const Link& foot, Index index, Side side, size_t rank = 0 );
			virtual ~Leg_Simbody();

			virtual Vec3 GetContactForce() const override;

		private:
			Model_Simbody& m_Model;
			int m_ForceIndex;
		};
	}
}
