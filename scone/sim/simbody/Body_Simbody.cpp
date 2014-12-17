#include "stdafx.h"
#include "Body_Simbody.h"
#include "..\..\core\Exception.h"

namespace scone
{
	namespace sim
	{
		Body_Simbody::Body_Simbody( OpenSim::Body& body ) :
		Body(),
		m_osBody( body )
		{
		}

		scone::Vec3 scone::sim::Body_Simbody::GetPos()
		{
			SCONE_THROW_NOT_IMPLEMENTED;
		}
		
		scone::Quat scone::sim::Body_Simbody::GetOri()
		{
			SCONE_THROW_NOT_IMPLEMENTED;
		}
		
		scone::Vec3 scone::sim::Body_Simbody::GetLinVel()
		{
			SCONE_THROW_NOT_IMPLEMENTED;
		}
		
		scone::Vec3 scone::sim::Body_Simbody::GetAngVel()
		{
			SCONE_THROW_NOT_IMPLEMENTED;
		}
}
}
