#include "stdafx.h"
#include "Joint_Simbody.h"
#include "..\..\core\Exception.h"


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
	}
}
