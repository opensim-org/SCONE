#pragma once

#include "sim_simbody.h"
#include "scone/sim/Dof.h"

namespace OpenSim
{
	class Coordinate;
}

namespace scone
{
	namespace sim
	{
		class SCONE_SIM_SIMBODY_API Dof_Simbody : public Dof
		{
		public:
			Dof_Simbody( class Model_Simbody& model, OpenSim::Coordinate& coord );
			virtual ~Dof_Simbody();

			virtual Real GetPos() const override;
			virtual Real GetVel() const override;
			virtual Real GetLimitForce() const override;

			virtual const String& GetName() const override;

			const OpenSim::Coordinate& GetOsCoordinate() const { return m_osCoord; }

			virtual void SetPos( Real pos, bool enforce_constraints = true ) override;
			virtual void SetVel( Real vel ) override;

			virtual Vec3 GetRotationAxis() const override;

		private:
			Model_Simbody& m_Model;
			OpenSim::Coordinate& m_osCoord;
			const OpenSim::CoordinateLimitForce* m_pOsLimitForce;
			Vec3 m_RotationAxis;

			friend class Model_Simbody;
		};
	}
}
