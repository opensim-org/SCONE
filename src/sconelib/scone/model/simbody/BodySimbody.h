#pragma once

#include "sim_simbody.h"
#include "scone/model/Body.h"
#include "ModelSimbody.h"

namespace OpenSim
{
	class Body;
}

namespace scone
{
	class SCONE_API BodySimbody : public Body
	{
	public:
		BodySimbody( class ModelSimbody& model, OpenSim::Body& body );
		virtual ~BodySimbody() { };

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

		OpenSim::Body& m_osBody;
		class ModelSimbody& m_Model;

		// connect a specific stance_contact force to this body
		void ConnectContactForce( const String& force_name );

		virtual Model& GetModel() override;
		virtual const Model& GetModel() const override;

		virtual std::vector< path > GetDisplayGeomFileNames() const override;

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
