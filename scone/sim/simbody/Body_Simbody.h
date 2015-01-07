#pragma once

#include "sim_simbody.h"
#include "../Body.h"

namespace OpenSim
{
	class Body;
}

namespace scone
{
	namespace sim
	{
		class SCONE_SIM_SIMBODY_API Body_Simbody : public Body
		{
		public:
			Body_Simbody( class Model_Simbody& model, OpenSim::Body& body );
			virtual ~Body_Simbody() { };

			virtual Vec3 GetPos() override;
			virtual Quat GetOri() override;

			virtual Vec3 GetLinVel() override;
			virtual Vec3 GetAngVel() override;

			virtual const String& GetName();

			OpenSim::Body& m_osBody;
			class Model_Simbody& m_Model;
		};
	}
}
