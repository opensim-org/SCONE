#pragma once

#include "scone/model/Dof.h"

namespace OpenSim
{
	class Coordinate;
	class CoordinateLimitForce;
	class CoordinateActuator;
}

namespace scone
{
	class SCONE_API Dof_Simbody : public Dof
	{
	public:
		Dof_Simbody( class Model_Simbody& model, OpenSim::Coordinate& coord );
		virtual ~Dof_Simbody();

		virtual Real GetPos() const override;
		virtual Real GetVel() const override;

		virtual Real GetLimitForce() const override;
		virtual Real GetMoment() const override;

		virtual const String& GetName() const override;
		const OpenSim::Coordinate& GetOsCoordinate() const { return m_osCoord; }

		void SetCoordinateActuator( OpenSim::CoordinateActuator* ca ) { m_OsCoordAct = ca; }

		virtual void SetPos( Real pos, bool enforce_constraints = true ) override;
		virtual void SetVel( Real vel ) override;

		virtual Vec3 GetRotationAxis() const override;
		virtual Range< Real > GetRange() const override;

	private:
		Model_Simbody& m_Model;
		OpenSim::Coordinate& m_osCoord;
		const OpenSim::CoordinateLimitForce* m_pOsLimitForce;
		const OpenSim::CoordinateActuator* m_OsCoordAct;
		Vec3 m_RotationAxis;

		friend class Model_Simbody;
	};
}
