/*
** BodyOpenSim3.h
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
	class ModelOpenSim3;
}

namespace scone
{
	class SCONE_OPENSIM_3_API BodyOpenSim3 : public Body
	{
	public:
		BodyOpenSim3( class ModelOpenSim3& model, OpenSim::Body& body );
		virtual ~BodyOpenSim3();;

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

		virtual bool HasContact() const override { return m_ForceIndex != -1; }
		virtual Vec3 GetContactForce() const override;
		virtual Vec3 GetContactMoment() const override;

		OpenSim::Body& m_osBody;
		class ModelOpenSim3& m_Model;

		// connect a specific stance_contact force to this body
		void ConnectContactForce( const String& force_name );

		virtual Model& GetModel() override;
		virtual const Model& GetModel() const override;

		virtual std::vector< DisplayGeometry > GetDisplayGeometries() const override;

		virtual const std::vector< Real >& GetContactForceValues() const override;
		virtual const std::vector< String >& GetContactForceLabels() const override { return m_ContactForceLabels; }

		virtual void SetExternalForce( const Vec3& f ) override;
		virtual void SetExternalMoment( const Vec3& torque ) override;
		virtual void AddExternalForce( const Vec3& f ) override;
		virtual void AddExternalMoment( const Vec3& torque ) override;
		virtual void SetExternalForceAtPoint( const Vec3& force, const Vec3& point ) override;

		virtual Vec3 GetExternalForce() const override;
		virtual Vec3 GetExternalMoment() const override;
		virtual Vec3 GetExternalForcePoint() const override;

	private:
		Vec3 m_LocalComPos;
		int m_ForceIndex;
		mutable int m_LastNumDynamicsRealizations;
		mutable std::vector< Real > m_ContactForceValues;
		std::vector< String > m_ContactForceLabels;
	};
}
