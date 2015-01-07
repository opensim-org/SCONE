#include "stdafx.h"
#include "Joint_Simbody.h"
#include "../../core/Exception.h"

#include <OpenSim/OpenSim.h>

namespace scone
{
	namespace sim
	{
		Joint_Simbody::Joint_Simbody( OpenSim::Joint& osJoint ) : m_osJoint( osJoint )
		{
		}

		Joint_Simbody::~Joint_Simbody()
		{
		}

		const String& Joint_Simbody::GetName()
		{
			return m_osJoint.getName();
		}

	}
}
