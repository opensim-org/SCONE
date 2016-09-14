#pragma once

#include "sim_simbody.h"
#include "scone/sim/Body.h"
#include "Model_Simbody.h"

namespace OpenSim
{
	class Body;
}

namespace scone
{
	namespace sim
	{
		class SCONE_API Body_Simbody : public Body
		{
		public:
			Body_Simbody( class Model_Simbody& model, OpenSim::Body& body );
			virtual ~Body_Simbody() { };

			virtual Vec3 GetOrigin() const override;
			virtual Vec3 GetComPos() const override;
			virtual Quat GetOri() const override;
			virtual Vec3 GetPosOfPointFixedOnBody(Vec3 point) const override;

			virtual Vec3 GetLinVel() const override;
			virtual Vec3 GetAngVel() const override;
			virtual Vec3 GetLinVelOfPointFixedOnBody(Vec3 point) const override;

			virtual const String& GetName() const override;

			virtual Vec3 GetContactForce() const override;
			virtual Vec3 GetContactMoment() const override;

			OpenSim::Body& m_osBody;
			class Model_Simbody& m_Model;

			// connect a specific stance_contact force to this body
			void ConnectContactForce( const String& force_name );

			virtual Model& GetModel() override;
			virtual const Model& GetModel() const override;

			virtual std::vector< String > GetDisplayGeomFileNames() const override;

			virtual const std::vector< Real >& GetContactForceValues() const override;
			virtual const std::vector< String >& GetContactForceLabels() const override { return m_ContactForceLabels; }

		private:
			int m_ForceIndex;
			mutable int m_LastNumDynamicsRealizations;
			mutable std::vector< Real > m_ContactForceValues;
			std::vector< String > m_ContactForceLabels;
		};
	}
}
