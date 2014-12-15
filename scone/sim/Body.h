#pragma once

#include "sim.h"
#include "..\core\Vec3.h"
#include "..\core\Quat.h"

namespace scone
{
	namespace sim
	{
		class SCONE_SIM_API Body
		{
		public:
			Body();
			virtual ~Body();

			virtual Vec3 GetPos() = 0;
			virtual Quat GetOri() = 0;

			virtual Vec3 GetLinVel() = 0;
			virtual Vec3 GetAngVel() = 0;

			virtual class Model& GetModel() = 0;

			virtual size_t GetParentJointCount() = 0;
			virtual class Joint& GetParentJoint( size_t idx ) = 0;
			virtual size_t GetChildJointCount() = 0;
			virtual class Joint& GetChildJoint( size_t idx ) = 0;

			virtual size_t GetParentLinkCount() = 0;
			virtual size_t GetParentLink( size_t idx ) = 0;
			virtual size_t GetChildLinkCount() = 0;
			virtual size_t GetChildLink( size_t idx ) = 0;
		};
	}
}
