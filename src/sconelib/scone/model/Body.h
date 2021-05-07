/*
** Body.h
**
** Copyright (C) 2013-2021 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "scone/core/HasName.h"
#include "scone/core/Vec3.h"
#include "scone/core/Quat.h"
#include "scone/core/system_tools.h"
#include "scone/core/HasData.h"
#include "DisplayGeometry.h"
#include "scone/core/PropNode.h"

namespace scone
{
	class Model;
	class Joint;

	class SCONE_API Body : public HasName, HasData
	{
	public:
		Body();
		virtual ~Body();

		virtual Real GetMass() const = 0;
		virtual Vec3 GetInertiaTensorDiagonal() const = 0;

		virtual Vec3 GetOriginPos() const = 0;
		virtual Vec3 GetComPos() const = 0;
		virtual Vec3 GetLocalComPos() const = 0;
		virtual Quat GetOrientation() const = 0;
		virtual Vec3 GetPosOfPointOnBody( Vec3 point ) const = 0;

		virtual Vec3 GetComVel() const = 0;
		virtual Vec3 GetOriginVel() const = 0;
		virtual Vec3 GetAngVel() const = 0;
		virtual Vec3 GetLinVelOfPointOnBody( Vec3 point ) const = 0;

		virtual Vec3 GetComAcc() const = 0;
		virtual Vec3 GetOriginAcc() const = 0;
		virtual Vec3 GetAngAcc() const = 0;
		virtual Vec3 GetLinAccOfPointOnBody( Vec3 point ) const = 0;

		virtual Vec3 GetContactForce() const = 0;
		virtual Vec3 GetContactMoment() const = 0;
		virtual Vec3 GetContactPoint() const = 0;

		virtual void SetExternalForce( const Vec3& force ) = 0;
		virtual void SetExternalForceAtPoint( const Vec3& force, const Vec3& point ) = 0;
		virtual void SetExternalMoment( const Vec3& torque ) = 0;
		virtual void AddExternalForce( const Vec3& f ) = 0;
		virtual void AddExternalMoment( const Vec3& torque ) = 0;

		virtual Vec3 GetExternalForce() const = 0;
		virtual Vec3 GetExternalForcePoint() const = 0;
		virtual Vec3 GetExternalMoment() const = 0;

		virtual Real GetPower() const { return 0.0; }

		virtual void ClearExternalForceAndMoment();

		virtual const Model& GetModel() const = 0;
		virtual Model& GetModel() = 0;

		const Joint* GetJoint() const { return m_Joint; }
		const Body* GetParentBody() const;

		virtual std::vector< DisplayGeometry > GetDisplayGeometries() const { return std::vector< DisplayGeometry >(); }

		virtual void StoreData( Storage<Real>::Frame& frame, const StoreDataFlags& flags ) const override;
		virtual PropNode GetInfo() const;

	protected:
		friend Joint;
		Joint* m_Joint; // set automatically when a Joint is created
	};
}
