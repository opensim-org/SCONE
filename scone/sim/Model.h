#pragma once

#include "../core/core.h"
#include "sim.h"
#include "../core/Vec3.h"
#include "Controller.h"
#include "Link.h"
#include "../opt/ParamSet.h"
#include "Leg.h"

namespace scone
{
	namespace sim
	{
		class SCONE_SIM_API Model
		{
		public:
			Model( const PropNode& props, opt::ParamSet& par );
			virtual ~Model();

			/// create model using parameters and properties
			virtual void Initialize( opt::ParamSet& par, const PropNode& props ) {};

			virtual Vec3 GetComPos() = 0;
			virtual Vec3 GetComVel() = 0;
			virtual Real GetMass() = 0;
			virtual Vec3 GetGravity() = 0;
			virtual bool HasGroundContact() = 0;

			/// muscle access
			size_t GetMuscleCount() { return m_Muscles.size(); }
			Muscle& GetMuscle( size_t idx ) { return *m_Muscles[ idx ]; }
			std::vector< MuscleUP >& GetMuscles() { return m_Muscles; }

			/// body access
			size_t GetBodyCount() { return m_Bodies.size(); }
			Body& GetBody( size_t idx ) { return *m_Bodies[ idx ]; }
			Body& FindBody( const String& name );
			std::vector< BodyUP >& GetBodies() { return m_Bodies; }

			/// joint access
			std::vector< JointUP >& GetJoints() { return m_Joints; }

			/// link access
			const Link& GetRootLink() const { return *m_RootLink; }

			/// controller access
			std::vector< ControllerUP >& GetControllers() { return m_Controllers; }

			/// leg access
			std::vector< LegUP >& GetLegs() { return m_Legs; }

			/// Simulate model
			virtual double GetTime() = 0;
			virtual size_t GetStep() = 0;
			virtual void AdvanceSimulationTo( double time ) = 0;
			virtual void WriteStateHistory( const String& file ) = 0;

			// TODO: perhaps remove termination request here
			virtual void SetTerminationRequest() { m_ShouldTerminate = true; }
			virtual bool GetTerminationRequest() { return m_ShouldTerminate; }

			// streaming operator (for debugging)
			virtual std::ostream& ToStream( std::ostream& str ) const;

		protected:
			LinkUP m_RootLink;
			std::vector< MuscleUP > m_Muscles;
			std::vector< BodyUP > m_Bodies;
			std::vector< JointUP > m_Joints;
			std::vector< LegUP > m_Legs;
			std::vector< ControllerUP > m_Controllers;
			bool m_ShouldTerminate;
		};
		
		inline std::ostream& operator<<( std::ostream& str, const Model& model ) { return model.ToStream( str ); }
	}
}
