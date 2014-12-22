#pragma once

#include "../core/core.h"
#include "sim.h"
#include "../core/Vec3.h"
#include "Controller.h"
#include "Link.h"

namespace scone
{
	namespace sim
	{
		class SCONE_SIM_API Model : public Propertyable
		{
		public:
			Model();
			virtual ~Model();

			virtual Vec3 GetComPos() = 0;
			virtual Vec3 GetComVel() = 0;
			virtual Real GetMass() = 0;

			std::vector< MuscleUP >& GetMuscles() { return m_Muscles; }
			std::vector< BodyUP >& GetBodies() { return m_Bodies; }
			std::vector< JointUP >& GetJoints() { return m_Joints; }

			Link& GetRootLink() { return *m_RootLink; }

			virtual void AddController( ControllerSP controller );
			std::vector< ControllerSP >& GetControllers() { return m_Controllers; }

			virtual void ProcessProperties( const PropNode& props ) override;

		protected:
			std::unique_ptr< Link > m_RootLink;
			std::vector< MuscleUP > m_Muscles;
			std::vector< BodyUP > m_Bodies;
			std::vector< JointUP > m_Joints;
			std::vector< ControllerSP > m_Controllers;

		private:
			Model( const Model& );
			Model& operator=( const Model& );
		};
	}
}
