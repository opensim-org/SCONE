/*
** BodyOpenSim4.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "platform.h"
#include "scone/model/Body.h"

namespace OpenSim
{
	class Body;
	class PhysicalFrame;
}

namespace scone
{
	class SCONE_OPENSIM_4_API BodyOpenSim4 : public Body
	{
	public:
		BodyOpenSim4( class ModelOpenSim4& model, const OpenSim::PhysicalFrame& body );
		virtual ~BodyOpenSim4();

		virtual Real GetMass() const override;
		virtual Vec3 GetInertiaTensorDiagonal() const override;

		virtual Vec3 GetOriginPos() const override;
		virtual Vec3 GetComPos() const override;
		virtual Vec3 GetLocalComPos() const override;
		virtual Quat GetOrientation() const override;
		virtual Vec3 GetPosOfPointOnBody( Vec3 point ) const override;

		virtual Vec3 GetComVel() const override;
		virtual Vec3 GetOriginVel() const override;
		virtual Vec3 GetAngVel() const override;
		virtual Vec3 GetLinVelOfPointOnBody( Vec3 point ) const override;

		virtual Vec3 GetComAcc() const override;
		virtual Vec3 GetOriginAcc() const override;
		virtual Vec3 GetAngAcc() const override;
		virtual Vec3 GetLinAccOfPointOnBody( Vec3 point ) const override;

		virtual const String& GetName() const override;

		virtual Vec3 GetContactForce() const override;
		virtual Vec3 GetContactMoment() const override;
		virtual Vec3 GetContactPoint() const override;

		virtual Model& GetModel() override;
		virtual const Model& GetModel() const override;

		virtual std::vector< DisplayGeometry > GetDisplayGeometries() const override;

		virtual void SetExternalForce( const Vec3& f ) override;
		virtual void SetExternalMoment( const Vec3& torque ) override;
		virtual void AddExternalForce( const Vec3& f ) override;
		virtual void AddExternalMoment( const Vec3& torque ) override;
		virtual void SetExternalForceAtPoint( const Vec3& force, const Vec3& point ) override;

		virtual Vec3 GetExternalForce() const override;
		virtual Vec3 GetExternalMoment() const override;
		virtual Vec3 GetExternalForcePoint() const override;

		const OpenSim::PhysicalFrame& GetOsBody() const { return m_osBody; }

		void AttachContactForce( ContactForce* cf ) { m_ContactForces.push_back( cf ); }

	private:
		const OpenSim::PhysicalFrame& m_osBody;
		const OpenSim::Body* m_pBody;
		class ModelOpenSim4& m_Model;
		Vec3 m_LocalComPos;
		std::vector< ContactForce* > m_ContactForces;
	};
}
