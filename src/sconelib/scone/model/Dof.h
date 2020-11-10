/*
** Dof.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "scone/core/HasName.h"
#include "scone/core/Vec3.h"
#include "scone/core/Range.h"
#include "Actuator.h"

namespace scone
{
	class Joint;

	class SCONE_API Dof : public Actuator
	{
	public:
		Dof( const Joint* j );
		virtual ~Dof();

		virtual Real GetPos() const = 0;
		virtual Real GetVel() const = 0;
		virtual Real GetAcc() const = 0;

		virtual Real GetLimitForce() const = 0;
		virtual Real GetMoment() const = 0;

		virtual void SetPos( Real pos, bool enforce_constraints = true ) = 0;
		virtual void SetVel( Real vel ) = 0;

		virtual Vec3 GetRotationAxis() const = 0;
		virtual const Joint* GetJoint() const { return m_Joint; }
		virtual Range< Real > GetRange() const = 0;

		virtual bool IsActuated() const { return false; }
		virtual Real GetMinInput() const override { return -1.0; }
		virtual Real GetMaxInput() const override { return 1.0; }
		virtual Real GetMinTorque() const { return 0.0; }
		virtual Real GetMaxTorque() const { return 0.0; }

	private:
		const Joint* m_Joint;
	};
}
