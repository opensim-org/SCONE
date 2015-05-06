#pragma once

#include "sim.h"

#include "../core/core.h"
#include "../core/Vec3.h"
#include "Controller.h"
#include "Link.h"
#include "../opt/ParamSet.h"
#include "Leg.h"

#include <map>
#include <vector>
#include "State.h"
#include "../core/HasName.h"
#include "../core/HasSignature.h"
#include "SensorDelayAdapter.h"
#include "Sensor.h"

namespace scone
{
	namespace sim
	{
		class SCONE_SIM_API Model : public HasName, public HasSignature
		{
		public:
			Model( const PropNode& props, opt::ParamSet& par );
			virtual ~Model();

			/// muscle access
			size_t GetMuscleCount() { return m_Muscles.size(); }
			Muscle& GetMuscle( size_t idx ) { return *m_Muscles[ idx ]; }
			std::vector< MuscleUP >& GetMuscles() { return m_Muscles; }

			/// body access
			size_t GetBodyCount() { return m_Bodies.size(); }
			Body& GetBody( size_t idx ) { return *m_Bodies[ idx ]; }
			Body& FindBody( const String& name );
			Index FindBodyIndex( const String& name );
			std::vector< BodyUP >& GetBodies() { return m_Bodies; }

			/// joint access
			std::vector< JointUP >& GetJoints() { return m_Joints; }

			/// dof access
			std::vector< DofUP >& GetDofs() { return m_Dofs; }

			/// link access
			const Link& GetRootLink() const { return *m_RootLink; }

			/// controller access
			std::vector< ControllerUP >& GetControllers() { return m_Controllers; }

			/// leg access
			size_t GetLegCount() { return m_Legs.size(); }
			Leg& GetLeg( size_t idx ) { return *m_Legs[ idx ]; }
			std::vector< LegUP >& GetLegs() { return m_Legs; }

			/// Get simulation info
			virtual double GetTime() = 0;
			virtual State GetState() = 0;
			virtual int GetIntegrationStep() = 0;
			virtual int GetPreviousIntegrationStep() = 0;
			virtual double GetPreviousTime() = 0;
			virtual double GetDeltaTime() { return GetTime() - GetPreviousTime(); }

			/// Simulate model
			virtual void AdvanceSimulationTo( double time ) = 0;
			virtual double GetSimulationEndTime() = 0;
			virtual String WriteStateHistory( const String& file_base ) = 0;

			// get model statistics
			virtual Vec3 GetComPos() = 0;
			virtual Vec3 GetComVel() = 0;
			virtual Real GetMass() = 0;
			virtual Vec3 GetGravity() = 0;
			virtual bool HasGroundContact() = 0;
			virtual Real GetTotalEnergyConsumption() { SCONE_THROW_NOT_IMPLEMENTED; }

			// TODO: perhaps remove termination request here
			virtual void SetTerminationRequest() { m_ShouldTerminate = true; }
			virtual bool GetTerminationRequest() { return m_ShouldTerminate; }

			// streaming operator (for debugging)
			virtual std::ostream& ToStream( std::ostream& str ) const;

			// create delayed sensors
			SensorDelayAdapter& AcquireSensorDelayAdapter( Sensor& source );

		protected:
			virtual String GetMainSignature() override { return GetName(); }
			void UpdateSensorDelayAdapters();

		protected:
			LinkUP m_RootLink;
			std::vector< MuscleUP > m_Muscles;
			std::vector< BodyUP > m_Bodies;
			std::vector< JointUP > m_Joints;
			std::vector< DofUP > m_Dofs;
			std::vector< LegUP > m_Legs;
			std::vector< ControllerUP > m_Controllers;
			bool m_ShouldTerminate;

			Storage< Real > m_SensorDelayStorage;
			std::vector< std::unique_ptr< SensorDelayAdapter > > m_SensorDelayAdapters;
		};
		
		inline std::ostream& operator<<( std::ostream& str, const Model& model ) { return model.ToStream( str ); }
	}
}
