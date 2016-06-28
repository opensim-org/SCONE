#pragma once

#include "scone/sim/sim_fwd.h"
#include "scone/core/HasName.h"
#include "scone/core/Vec3.h"

namespace scone
{
	namespace sim
	{
		class SCONE_API Dof : public HasName
		{
		public:
			Dof( Joint& j );
			virtual ~Dof();

			virtual Real GetPos() const = 0;
			virtual Real GetVel() const = 0;

			virtual Real GetLimitForce() const = 0;

			virtual void SetPos( Real pos, bool enforce_constraints = true ) = 0;
			virtual void SetVel( Real vel ) = 0;

			virtual Vec3 GetRotationAxis() const = 0;
			virtual const Joint& GetJoint() const { return m_Joint; }

		private:
			Joint& m_Joint;
		};
	}
}
