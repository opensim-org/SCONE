#pragma once

#include "sim_simbody.h"
#include "..\Body.h"


namespace scone
{
	namespace sim
	{
		class SCONE_SIM_SIMBODY_API Body_Simbody : public Body
		{
		public:
			Body_Simbody( ) { };
			Body_Simbody( const class Model& model ) { };
			virtual ~Body_Simbody() { };

			virtual Vec3 GetPos() override;
			virtual Quat GetOri() override;

			virtual Vec3 GetLinVel() override;
			virtual Vec3 GetAngVel() override;

			virtual class Model& GetModel() override;

			virtual size_t GetParentJointCount() override;
			virtual class Joint& GetParentJoint( size_t idx ) override;
			virtual size_t GetChildJointCount() override;
			virtual class Joint& GetChildJoint( size_t idx ) override;

			virtual size_t GetParentLinkCount() override;
			virtual size_t GetParentLink( size_t idx ) override;
			virtual size_t GetChildLinkCount() override;
			virtual size_t GetChildLink( size_t idx ) override;
		};
	}
}
