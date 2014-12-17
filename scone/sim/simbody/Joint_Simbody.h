#pragma once

#include "sim_simbody.h"
#include "..\Joint.h"

namespace OpenSim
{
	class Joint;
}

namespace scone
{
	namespace sim
	{
		class SCONE_SIM_SIMBODY_API Joint_Simbody : public Joint
		{
		public:
			Joint_Simbody( OpenSim::Joint& osJoint );
			virtual ~Joint_Simbody();

		private:
			OpenSim::Joint& m_osJoint;
		};
	}
}
