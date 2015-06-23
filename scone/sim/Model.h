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
#include "ChannelSensorDelayAdapter.h"
#include "ChannelSensor.h"
#include "BalanceSensor.h"

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
			std::vector< MuscleUP >& GetMuscles() { return m_Muscles; }
			const std::vector< MuscleUP >& GetMuscles() const { return m_Muscles; }

			/// body access
			std::vector< BodyUP >& GetBodies() { return m_Bodies; }
			const std::vector< BodyUP >& GetBodies() const { return m_Bodies; }

			/// joint access
			std::vector< JointUP >& GetJoints() { return m_Joints; }
			const std::vector< JointUP >& GetJoints() const { return m_Joints; }

			/// dof access
			std::vector< DofUP >& GetDofs() { return m_Dofs; }
			const std::vector< DofUP >& GetDofs() const { return m_Dofs; }

			/// Sensor access
			std::vector< ChannelSensor* >& GetChannelSensors() { return m_ChannelSensors; }
			//std::vector< ChannelSensor* >& GetChannelSensors() { return m_ChannelSensors; }
			std::vector< Actuator* >& GetActuators() { return m_Actuators; }

			/// link access
			const Link& GetRootLink() const { return *m_RootLink; }

			/// controller access
			std::vector< ControllerUP >& GetControllers() { return m_Controllers; }
			const std::vector< ControllerUP >& GetControllers() const { return m_Controllers; }

			void UpdateControlValues();
			void UpdateAnalyses();

			/// leg access
			size_t GetLegCount() const { return m_Legs.size(); }
			const Leg& GetLeg( size_t idx ) const { return *m_Legs[ idx ]; }
			std::vector< LegUP >& GetLegs() { return m_Legs; }
			const std::vector< LegUP >& GetLegs() const { return m_Legs; }

			/// Get simulation info
			virtual double GetTime() const = 0;
			virtual State GetState() const = 0;
			virtual void SetState( const State& state ) = 0;
			virtual int GetIntegrationStep() const = 0;
			virtual int GetPreviousIntegrationStep() const = 0;
			virtual double GetPreviousTime() const = 0;
			virtual double GetDeltaTime() const { return GetTime() - GetPreviousTime(); }

			/// Simulate model
			virtual void AdvanceSimulationTo( double time ) = 0;
			virtual double GetSimulationEndTime() const = 0;
			virtual String WriteStateHistory( const String& file_base ) const = 0;

			// get model statistics
			virtual Vec3 GetComPos() const = 0;
			virtual Vec3 GetComVel() const = 0;
			virtual Real GetMass() const = 0;
			virtual Vec3 GetGravity() const = 0;
			virtual Real GetTotalEnergyConsumption() const { SCONE_THROW_NOT_IMPLEMENTED; }

			// TODO: perhaps remove termination request here
			virtual void SetTerminationRequest() { m_ShouldTerminate = true; }
			virtual bool GetTerminationRequest() { return m_ShouldTerminate; }

			// streaming operator (for debugging)
			virtual std::ostream& ToStream( std::ostream& str ) const;

			// create delayed sensors
			SensorDelayAdapter& AcquireSensorDelayAdapter( Sensor& source );
			Storage< Real >& GetSensorDelayStorage() { return m_SensorDelayStorage; }

		protected:
			virtual String GetClassSignature() const override { return GetName(); }
			void UpdateSensorDelayAdapters();

		protected:
			LinkUP m_RootLink;
			std::vector< MuscleUP > m_Muscles;
			std::vector< BodyUP > m_Bodies;
			std::vector< JointUP > m_Joints;
			std::vector< DofUP > m_Dofs;
			std::vector< LegUP > m_Legs;
			std::vector< ControllerUP > m_Controllers;
			//BalanceSensorUP m_BalanceSensor;
			bool m_ShouldTerminate;

			// non-owning storage
			std::vector< ChannelSensor* > m_ChannelSensors;
			std::vector< Actuator* > m_Actuators;
			Storage< Real > m_SensorDelayStorage;
			std::vector< std::unique_ptr< SensorDelayAdapter > > m_SensorDelayAdapters;
		};
		
		inline std::ostream& operator<<( std::ostream& str, const Model& model ) { return model.ToStream( str ); }
	}
}
