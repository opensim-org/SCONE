#pragma once

#include "scone/core/core.h"
#include "scone/opt/opt_fwd.h"
#include "scone/sim/sim_fwd.h"

#include "Controller.h"
#include "Link.h"
#include "Leg.h"

#include <vector>
#include "scone/core/HasName.h"
#include "scone/core/HasSignature.h"
#include "Sensor.h"
#include "scone/core/Storage.h"
#include <array>
#include <type_traits>
#include <mutex>
#include <condition_variable>

namespace scone
{
	namespace sim
	{
		class SCONE_API Model : public HasName, public HasSignature, public HasData
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

			/// Model state access
			virtual std::vector< Real > GetStateValues() const = 0;
			virtual void SetStateValues( const std::vector< Real >& values ) = 0;
			virtual std::vector< String > GetStateVariableNames() const = 0;
			virtual Real GetStateVariable( const String& name ) const = 0;
			virtual void SetStateVariable( const String& name, Real value ) = 0;

			/// Simulate model
			virtual bool AdvanceSimulationTo( double time ) = 0;
			virtual double GetSimulationEndTime() const = 0;
			virtual void SetSimulationEndTime( double time ) = 0;

			/// Model data
			virtual const Storage< Real, TimeInSeconds > GetData() { return m_Data; }
			virtual String WriteData( const String& file_base ) const = 0;

			/// get dynamic model statistics
			virtual Vec3 GetComPos() const = 0;
			virtual Vec3 GetComVel() const = 0;
            virtual Vec3 GetComAcc() const = 0;
			virtual Real GetTotalEnergyConsumption() const { SCONE_THROW_NOT_IMPLEMENTED; }
			virtual Real GetTotalContactForce() const;

			// get static model info
			virtual Real GetMass() const = 0;
			virtual Vec3 GetGravity() const = 0;
			virtual Real GetBW() const;
			
			// custom model properties
			template< typename T >
			T GetCustomProp( const String& key, const T& default_value ) const { return custom_properties.Get( key, default_value );  }
			const PropNode& GetCustomProps() { return custom_properties; }

			// TODO: perhaps remove termination request here
			virtual void SetTerminationRequest() { m_ShouldTerminate = true; }
			virtual bool GetTerminationRequest() { return m_ShouldTerminate; }

			// streaming operator (for debugging)
			virtual std::ostream& ToStream( std::ostream& str ) const;

			// acquire a sensor of type SensorT with a source of type SourceT
			template< typename SensorT, typename... Args >
			SensorT& AcquireSensor( Args&&... args )
			{
				static_assert( std::is_base_of< Sensor, SensorT >::value, "SensorT is not derived from Sensor" );

				// create a new sensor and see if there's an existing sensor of same type with same source name
				SensorUP sensor = SensorUP( new SensorT( std::forward< Args >( args )... ) );
				auto it = std::find_if( m_Sensors.begin(), m_Sensors.end(),[&]( SensorUP& s ) {
					return dynamic_cast< SensorT* >( s.get() ) != nullptr && s->GetName() == sensor->GetName(); } );

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
			Real balance_sensor_ori_vel_gain;
			Real balance_sensor_delay;
			Vec3 GetDelayedOrientation();

			virtual void SetStoreData( bool store ) { m_StoreData = store; }
			virtual bool GetStoreData() { return m_StoreData; }

			void SetThreadSafeSimulation( bool b ) { thread_safe_simulation = b; }
			bool GetThreadSafeSimulation() { return thread_safe_simulation; }
			std::mutex& GetSimulationMutex() { return simulation_mutex; }
			std::condition_variable& GetSimulationCondVar() { return simulation_cv; }

		protected:
			virtual String GetClassSignature() const override { return GetName(); }
			void UpdateSensorDelayAdapters();
			void CreateBalanceSensors( const PropNode& props, opt::ParamSet& par );

			virtual void StoreData( Storage< Real >::Frame& frame ) override;
			virtual void StoreCurrentFrame();

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
			const PropNode& custom_properties;

			// storage for HasData classes
			Storage< Real, TimeInSeconds > m_Data;
			bool m_StoreData;

			// thread safety stuff
			bool thread_safe_simulation;
			std::mutex simulation_mutex;
			std::condition_variable simulation_cv;
		};
		
		inline std::ostream& operator<<( std::ostream& str, const Model& model ) { return model.ToStream( str ); }
	}
}
