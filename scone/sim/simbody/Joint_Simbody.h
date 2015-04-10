#pragma once

#include "sim_simbody.h"
#include "../Joint.h"

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
			Joint_Simbody( class Model_Simbody& model, OpenSim::Joint& osJoint );
			virtual ~Joint_Simbody();

			virtual const String& GetName() const;

			virtual size_t GetDofCount() override;
			virtual Real GetDof( size_t index = 0 ) override;
			virtual const String& GetDofName( size_t index = 0 ) override;

			class Model_Simbody& m_Model;
			OpenSim::Joint& m_osJoint;
		};
	}
}
