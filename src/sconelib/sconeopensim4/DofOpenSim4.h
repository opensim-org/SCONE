/*
** DofOpenSim4.h
**
** Copyright (C) 2013-2018 Thomas Geijtenbeek. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "scone/model/Dof.h"
#include "platform.h"

namespace OpenSim
{
	class Coordinate;
	class CoordinateLimitForce;
	class CoordinateActuator;
}

namespace scone
{
	class SCONE_OPENSIM_4_API DofOpenSim4 : public Dof
	{
	public:
		DofOpenSim4( class ModelOpenSim4& model, OpenSim::Coordinate& coord );
		virtual ~DofOpenSim4();

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
		ModelOpenSim4& m_Model;
		OpenSim::Coordinate& m_osCoord;
		const OpenSim::CoordinateLimitForce* m_pOsLimitForce;
		const OpenSim::CoordinateActuator* m_OsCoordAct;
		Vec3 m_RotationAxis;

		friend class ModelOpenSim4;
	};
}
