#pragma once

#include "sim.h"

#include "../core/core.h"
#include "../core/Vec3.h"
#include "Controller.h"
#include "Link.h"
#include "Leg.h"

#include "../opt/ParamSet.h"

#include <map>
#include <vector>
#include "State.h"
#include "../core/HasName.h"
#include "../core/HasSignature.h"
#include "Sensor.h"
#include "../core/Storage.h"
#include <array>
#include <type_traits>

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
			//std::vector< ChannelSensor* >& GetChannelSensors() { return m_ChannelSensors; }
			std::vector< Actuator* >& GetActuators() { return m_Actuators; }

			/// link access
			const Link& FindLink( const String& body_name );
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
			virtual int GetIntegrationStep() const = 0;
			virtual int GetPreviousIntegrationStep() const = 0;
			virtual double GetPreviousTime() const = 0;
			virtual double GetDeltaTime() const { return GetTime() - GetPreviousTime(); }

			// Model state access
			virtual std::vector< Real > GetStateValues() const = 0;
			virtual void SetStateValues( const std::vector< Real >& values ) = 0;
			virtual std::vector< String > GetStateVariableNames() const = 0;
			virtual Real GetStateVariable( const String& name ) const = 0;
			virtual void SetStateVariable( const String& name, Real value ) = 0;

			/// Simulate model
			virtual bool AdvanceSimulationTo( double time ) = 0;
			virtual double GetSimulationEndTime() const = 0;
			virtual String WriteStateHistory( const String& file_base ) const = 0;

			// get model statistics
			virtual Vec3 GetComPos() const = 0;
			virtual Vec3 GetComVel() const = 0;
			virtual Real GetMass() const = 0;
			virtual Vec3 GetGravity() const = 0;
			virtual Real GetTotalEnergyConsumption() const { SCONE_THROW_NOT_IMPLEMENTED; }
			virtual Real GetTotalContactForce() const;
			
			// custom model properties
			template< typename T >
			T GetCustomProp( const String& key, const T& default_value ) const { return custom_properties.Get( key, default_value );  }

			// TODO: perhaps remove termination request here
			virtual void SetTerminationRequest() { m_ShouldTerminate = true; }
			virtual bool GetTerminationRequest() { return m_ShouldTerminate; }

			// streaming operator (for debugging)
			virtual std::ostream& ToStream( std::ostream& str ) const;

			// acquire a sensor of type SensorT with a source of type SourceT
			// TODO: use variadic arguments and perfect forwarding
			template< typename SensorT, typename... Args >
			SensorT& AcquireSensor( Args&&... args )
			{
				static_assert( std::is_base_of< Sensor, SensorT >::value, "SensorT is not derived from Sensor" );

				// create a new sensor and see if there's an existing sensor of same type with same source name
				SensorUP sensor = SensorUP( new SensorT( std::forward< Args >( args )... ) );
				auto it = std::find_if( m_Sensors.begin(), m_Sensors.end(),[&]( SensorUP& s ) { return typeid( *s ) == typeid( SensorT ) && s->GetName() == sensor->GetName(); } );
				if ( it == m_Sensors.end() )
				{
					// its new, so move it to the back of the container
					m_Sensors.push_back( std::move( sensor ) );
					return dynamic_cast< SensorT& >( *m_Sensors.back() ); // return new sensor
				}
				else return dynamic_cast< SensorT& >( **it ); // return found element
			}

			// acquire sensor based on PropNode
			Sensor& AcquireSensor( const PropNode& pn, opt::ParamSet& par, const sim::Area& area );
			SensorDelayAdapter& AcquireDelayedSensor( const PropNode& pn, opt::ParamSet& par, const sim::Area& area );

			// create delayed sensors
			SensorDelayAdapter& AcquireSensorDelayAdapter( Sensor& source );
			Storage< Real >& GetSensorDelayStorage() { return m_SensorDelayStorage; }


			template< typename SensorT, typename... Args >
			SensorDelayAdapter& AcquireDelayedSensor( Args&&... args ) {
				return AcquireSensorDelayAdapter( AcquireSensor< SensorT >( std::forward< Args >( args )... ) );
			}

			Real sensor_delay_scaling_factor;
			Vec3 GetDelayedOrientation();

		protected:
			virtual String GetClassSignature() const override { return GetName(); }
			void UpdateSensorDelayAdapters();
			void CreateBalanceSensors( const PropNode& props, opt::ParamSet& par );

		protected:
			LinkUP m_RootLink;
			std::vector< MuscleUP > m_Muscles;
			std::vector< BodyUP > m_Bodies;
			std::vector< JointUP > m_Joints;
			std::vector< DofUP > m_Dofs;
			std::vector< LegUP > m_Legs;
			std::vector< ControllerUP > m_Controllers;
			bool m_ShouldTerminate;

			// non-owning storage
			std::vector< Actuator* > m_Actuators;
			Storage< Real > m_SensorDelayStorage;
			std::vector< std::unique_ptr< SensorDelayAdapter > > m_SensorDelayAdapters;
			std::vector< std::unique_ptr< Sensor > > m_Sensors;

			std::array< SensorDelayAdapter*, 3 > m_OriSensors;
			Real balance_sensor_delay;
			Real balance_sensor_orientation_velocity_gain;
			const PropNode& custom_properties;
		};
		
		inline std::ostream& operator<<( std::ostream& str, const Model& model ) { return model.ToStream( str ); }
	}
}
