#include "stdafx.h"
#include "Body_Simbody.h"
#include "..\..\core\Exception.h"

namespace scone
{
	namespace sim
	{
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
		
		class Model& scone::sim::Body_Simbody::GetModel()
		{
			SCONE_THROW_NOT_IMPLEMENTED;
		}
		
		size_t scone::sim::Body_Simbody::GetParentJointCount()
		{
			SCONE_THROW_NOT_IMPLEMENTED;
		}
		
		class Joint& scone::sim::Body_Simbody::GetParentJoint(size_t idx)
		{
			SCONE_THROW_NOT_IMPLEMENTED;
		}
		
		size_t scone::sim::Body_Simbody::GetChildJointCount()
		{
			SCONE_THROW_NOT_IMPLEMENTED;
		}
		
		class Joint& scone::sim::Body_Simbody::GetChildJoint(size_t idx)
		{
			SCONE_THROW_NOT_IMPLEMENTED;
		}
		
		size_t scone::sim::Body_Simbody::GetParentLinkCount()
		{
			SCONE_THROW_NOT_IMPLEMENTED;
		}
		
		size_t scone::sim::Body_Simbody::GetParentLink(size_t idx)
		{
			SCONE_THROW_NOT_IMPLEMENTED;
		}
		
		size_t scone::sim::Body_Simbody::GetChildLinkCount()
		{
			SCONE_THROW_NOT_IMPLEMENTED;
		}
		
		size_t scone::sim::Body_Simbody::GetChildLink(size_t idx)
		{
			SCONE_THROW_NOT_IMPLEMENTED;
		}
	}
}
