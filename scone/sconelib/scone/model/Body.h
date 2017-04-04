#pragma once

#include "scone/model/sim_fwd.h"
#include "scone/core/HasName.h"
#include "scone/core/Vec3.h"
#include "scone/core/Quat.h"

namespace scone
{
	namespace sim
	{
		class SCONE_API Body : public HasName
		{
		public:
			Body();
			virtual ~Body();

			virtual Vec3 GetOriginPos() const = 0;
			virtual Vec3 GetComPos() const = 0;
			virtual Vec3 GetLocalComPos() const = 0;
			virtual Quat GetOrientation() const = 0;
			virtual Vec3 GetPosOfPointFixedOnBody(Vec3 point) const = 0;

			virtual Vec3 GetComVel() const = 0;
			virtual Vec3 GetOriginVel() const = 0;
			virtual Vec3 GetAngVel() const = 0;
			virtual Vec3 GetLinVelOfPointFixedOnBody(Vec3 point) const = 0;

            virtual Vec3 GetComAcc() const = 0;
			virtual Vec3 GetOriginAcc() const = 0;
			virtual Vec3 GetAngAcc() const = 0;
            virtual Vec3 GetLinAccOfPointFixedOnBody(Vec3 point) const = 0;

			virtual const std::vector< Real >& GetContactForceValues() const = 0;
			virtual const std::vector< String >& GetContactForceLabels() const = 0;

			virtual Vec3 GetContactForce() const = 0;
			virtual Vec3 GetContactMoment() const = 0;

			virtual void SetExternalForce( const Vec3& force ) = 0;
			virtual void SetExternalForceAtPoint( const Vec3& force, const Vec3& point ) = 0;
			virtual void SetExternalTorque( const Vec3& torque ) = 0;

			virtual Vec3 GetExternalForce() const = 0;
			virtual Vec3 GetExternalForcePoint() const = 0;
			virtual Vec3 GetExternalTorque() const = 0;

			virtual const Model& GetModel() const = 0;
			virtual Model& GetModel() = 0;

			virtual std::vector< String > GetDisplayGeomFileNames() const { return std::vector< String >(); }
		};
	}
}
