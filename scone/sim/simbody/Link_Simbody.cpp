#include "stdafx.h"
#include "Link_Simbody.h"
#include "..\..\core\exceptions.h"

namespace scone
{
	namespace sim
	{
		scone::Vec3 scone::sim::Link_Simbody::GetPos()
		{
			SCONE_THROW_NOT_IMPLEMENTED;
		}
		
		scone::Quat scone::sim::Link_Simbody::GetOri()
		{
			SCONE_THROW_NOT_IMPLEMENTED;
		}
		
		scone::Vec3 scone::sim::Link_Simbody::GetLinVel()
		{
			SCONE_THROW_NOT_IMPLEMENTED;
		}
		
		scone::Vec3 scone::sim::Link_Simbody::GetAngVel()
		{
			SCONE_THROW_NOT_IMPLEMENTED;
		}
		
		class Model& scone::sim::Link_Simbody::GetModel()
		{
			SCONE_THROW_NOT_IMPLEMENTED;
		}
		
		size_t scone::sim::Link_Simbody::GetParentJointCount()
		{
			SCONE_THROW_NOT_IMPLEMENTED;
		}
		
		class Joint& scone::sim::Link_Simbody::GetParentJoint(size_t idx)
		{
			SCONE_THROW_NOT_IMPLEMENTED;
		}
		
		size_t scone::sim::Link_Simbody::GetChildJointCount()
		{
			SCONE_THROW_NOT_IMPLEMENTED;
		}
		
		class Joint& scone::sim::Link_Simbody::GetChildJoint(size_t idx)
		{
			SCONE_THROW_NOT_IMPLEMENTED;
		}
		
		size_t scone::sim::Link_Simbody::GetParentLinkCount()
		{
			SCONE_THROW_NOT_IMPLEMENTED;
		}
		
		size_t scone::sim::Link_Simbody::GetParentLink(size_t idx)
		{
			SCONE_THROW_NOT_IMPLEMENTED;
		}
		
		size_t scone::sim::Link_Simbody::GetChildLinkCount()
		{
			SCONE_THROW_NOT_IMPLEMENTED;
		}
		
		size_t scone::sim::Link_Simbody::GetChildLink(size_t idx)
		{
			SCONE_THROW_NOT_IMPLEMENTED;
		}
	}
}
