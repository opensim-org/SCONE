/*
** Model.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "scone/core/platform.h"
#include "scone/core/types.h"
#include "xo/filesystem/path.h"
#include "xo/system/profiler.h"

#include "ContactForce.h"
#include "ContactGeometry.h"
#include "ForceValue.h"
#include "Leg.h"
#include "Sensor.h"
#include "ModelFeatures.h"

#include "scone/controllers/Controller.h"
#include "scone/core/HasExternalResources.h"
#include "scone/core/HasName.h"
#include "scone/core/HasSignature.h"
#include "scone/core/Storage.h"
#include "scone/measures/Measure.h"
#include "scone/core/Factories.h"

#include <vector>
#include <type_traits>
#include <utility>

namespace scone
{
	/// Simulation model.
	class SCONE_API Model : public HasName, public HasSignature, public HasData, public HasExternalResources
	{
	public:
		Model( const PropNode& props, Params& par );
		virtual ~Model();

		// muscle access
		std::vector< MuscleUP >& GetMuscles() { return m_Muscles; }
		const std::vector< MuscleUP >& GetMuscles() const { return m_Muscles; }

		// body access
		std::vector< BodyUP >& GetBodies() { return m_Bodies; }
		const std::vector< BodyUP >& GetBodies() const { return m_Bodies; }
		const Body* GetRootBody() const { return m_RootBody; }

		// joint access
		const std::vector< JointUP >& GetJoints() const { return m_Joints; }

		// dof access
		std::vector< DofUP >& GetDofs() { return m_Dofs; }
		const std::vector< DofUP >& GetDofs() const { return m_Dofs; }

		// Actuator access
		std::vector< Actuator* >& GetActuators() { return m_Actuators; }
		const std::vector< Actuator* >& GetActuators() const { return m_Actuators; }

		// Contact geometries
		const std::vector< ContactGeometryUP >& GetContactGeometries() const { return m_ContactGeometries; }

		// Contact forces
		const std::vector< ContactForceUP >& GetContactForces() const { return m_ContactForces; }

		// Contact force values
		virtual std::vector< ForceValue > GetContactForceValues() const;

		// Model file access
		virtual path GetModelFile() const { return path(); }

		// Controller access
		Controller* GetController() { return m_Controller.get(); }
		const Controller* GetController() const { return m_Controller.get(); }
		virtual void SetController( ControllerUP c ) { SCONE_ASSERT( !m_Controller ); m_Controller = std::move( c ); }
		void CreateController( const FactoryProps& controller_fp, Params& par );

		// Measure access
		Measure* GetMeasure() { return m_Measure.get(); }
		const Measure* GetMeasure() const { return m_Measure.get(); }
		void SetMeasure( MeasureUP m ) { SCONE_ASSERT( !m_Measure ); m_Measure = std::move( m ); }
		void CreateMeasure( const FactoryProps& measure_fp, Params& par );

		void UpdateControlValues();
		void UpdateAnalyses();

		// leg access
		size_t GetLegCount() const { return m_Legs.size(); }
		const Leg& GetLeg( size_t idx ) const { return *m_Legs[ idx ]; }
		const Leg& GetLeg( const Location& loc ) const { for ( auto& l : m_Legs ) if ( l->GetSide() == loc.GetSide() ) return *l; xo_error( "Could not find leg" ); }
		std::vector< LegUP >& GetLegs() { return m_Legs; }
		const std::vector< LegUP >& GetLegs() const { return m_Legs; }

		// Get simulation info
		virtual TimeInSeconds GetTime() const = 0;
		virtual int GetIntegrationStep() const = 0;
		virtual int GetPreviousIntegrationStep() const = 0;
		virtual TimeInSeconds GetPreviousTime() const = 0;
		virtual TimeInSeconds GetDeltaTime() const { return GetTime() - GetPreviousTime(); }
		virtual TimeInSeconds GetSimulationStepSize() = 0;

		// Model state access
		virtual const State& GetState() const = 0;
		virtual void SetState( const State& state, TimeInSeconds timestamp ) = 0;
		virtual void SetStateValues( const std::vector< Real >& state, TimeInSeconds timestamp ) = 0;
		virtual void SetNullState();
		virtual void SetNeutralState();

		// Simulate model
		virtual void AdvanceSimulationTo( double time ) = 0;
		virtual double GetSimulationEndTime() const = 0;
		virtual void SetSimulationEndTime( double time ) = 0;
		virtual bool HasSimulationEnded() { return m_ShouldTerminate || GetTime() >= GetSimulationEndTime(); }
		virtual void RequestTermination() { m_ShouldTerminate = true; }
		virtual PropNode GetSimulationReport() const { return PropNode(); }
		virtual void UpdatePerformanceStats( const path& filename ) const {}
		virtual std::vector<std::pair<String, std::pair<xo::time, size_t>>> GetBenchmarks() const { return {}; }

		// Model data
		virtual const Storage< Real, TimeInSeconds >& GetData() const { return m_Data; }
		virtual std::vector<path> WriteResults( const path& file_base ) const;

		// get dynamic model statistics
		virtual Vec3 GetComPos() const = 0;
		virtual Vec3 GetComVel() const = 0;
		virtual Vec3 GetComAcc() const = 0;
		virtual Real GetComHeight( const Vec3& up = Vec3::unit_y() ) const;
		virtual Vec3 GetLinMom() const = 0;
		virtual Vec3 GetAngMom() const = 0;
		virtual std::pair<Vec3, Vec3> GetLinAngMom() const { return { GetLinMom(), GetAngMom() }; }
		virtual Real GetTotalEnergyConsumption() const { SCONE_THROW_NOT_IMPLEMENTED; }
		virtual Real GetTotalContactForce() const;

		// get static model info
		virtual Real GetMass() const = 0;
		virtual Vec3 GetGravity() const = 0;
		virtual Real GetBW() const;
		virtual const ContactGeometry* GetGroundPlane() const;
		virtual Vec3 GetProjectedOntoGround( const Vec3& point, const Vec3& up = Vec3::unit_y() ) const;

		// custom model properties
		PropNode& GetUserData() { return m_UserData; }
		virtual PropNode GetInfo() const;

		// features supported by this model
		virtual const ModelFeatures& GetFeatures() const { return m_Features; }

		// acquire a sensor of type SensorT with a source of type SourceT
		template< typename SensorT, typename... Args > SensorT& AcquireSensor( Args&&... args ) {
			static_assert( std::is_base_of< Sensor, SensorT >::value, "SensorT is not derived from Sensor" );
			// create a new sensor and see if there's an existing sensor of same type with same source name
			SensorUP sensor = SensorUP( new SensorT( std::forward< Args >( args )... ) );
			auto it = std::find_if( m_Sensors.begin(), m_Sensors.end(), [&]( SensorUP& s ) {
				return dynamic_cast<SensorT*>( s.get() ) != nullptr && s->GetName() == sensor->GetName(); } );
			if ( it == m_Sensors.end() ) {
				// its new, so move it to the back of the container
				m_Sensors.push_back( std::move( sensor ) );
				return dynamic_cast<SensorT&>( *m_Sensors.back() ); // return new sensor
			}
			else return dynamic_cast<SensorT&>( **it ); // return found element
		}

		// create delayed sensors
		SensorDelayAdapter& AcquireSensorDelayAdapter( Sensor& source );
		Storage< Real >& GetSensorDelayStorage() { return m_SensorDelayStorage; }

		template< typename SensorT, typename... Args > SensorDelayAdapter& AcquireDelayedSensor( Args&&... args )
		{ return AcquireSensorDelayAdapter( AcquireSensor< SensorT >( std::forward< Args >( args )... ) ); }

		/// Offset [rad] or [m] to apply to initial state; default = 0.
		const PropNode* initial_state_offset;

		/// Use symmetric offset for left and right; default = 0.
		bool initial_state_offset_symmetric;

		/// Pattern matching the states to include in initial offset (semicolon seperated); default = "*".
		String initial_state_offset_include;

		/// Pattern matching the states to exclude in initial offset (semicolon seperated); default = "".
		String initial_state_offset_exclude;

		/// Maximum integration step size; default = 0.001.
		double max_step_size;

		/// Use fixed step size for controllers; default = true.
		bool use_fixed_control_step_size;

		/// Step size used for controllers; default = 0.001.
		double fixed_control_step_size;

		/// Step size used for measures (not supported by all model types); default = ''fixed_control_step_size''.
		double fixed_measure_step_size;

		/// Initial load [BW] at which to place the model initially; default = 0.2;
		Real initial_load;

		/// Name of the DOF that needs to be adjusted to find the required initial_load; default = pelvis_ty.
		String initial_load_dof;

		/// Scaling factor to apply to all sensor delays; default = 1.
		Real sensor_delay_scaling_factor;

		/// Activation used to equilibrate muscles before control inputs are known; default = 0.05
		Real initial_equilibration_activation;

		void SetStoreData( bool store ) { m_StoreData = store; }
		bool GetStoreData() const;
		StoreDataFlags& GetStoreDataFlags() { return m_StoreDataFlags; }
		const StoreDataFlags& GetStoreDataFlags() const { return m_StoreDataFlags; }

		xo::profiler& GetProfiler() const { return m_Profiler; }

	protected:
		virtual String GetClassSignature() const override;
		void UpdateSensorDelayAdapters();
		void CreateControllers( const PropNode& pn, Params& par );

		virtual void StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags ) const override;
		virtual void StoreCurrentFrame();

		virtual void AddExternalDisplayGeometries( const path& model_path );

	protected:
		mutable xo::profiler m_Profiler;

		std::vector< MuscleUP > m_Muscles;
		std::vector< BodyUP > m_Bodies;
		std::vector< JointUP > m_Joints;
		std::vector< DofUP > m_Dofs;
		std::vector< LegUP > m_Legs;
		std::vector< ContactGeometryUP > m_ContactGeometries;
		std::vector< ContactForceUP > m_ContactForces;

		MeasureUP m_Measure;
		ControllerUP m_Controller;
		bool m_ShouldTerminate;

		// step size
		double fixed_step_size;
		int fixed_control_step_interval;
		int fixed_analysis_step_interval;

		// non-owning storage
		std::vector< Actuator* > m_Actuators;
		Storage< Real > m_SensorDelayStorage;
		std::vector< std::unique_ptr< SensorDelayAdapter > > m_SensorDelayAdapters;
		std::vector< std::unique_ptr< Sensor > > m_Sensors;
		Body* m_RootBody;

		const PropNode* m_pModelProps;
		const PropNode* m_pCustomProps;
		PropNode m_UserData;
		ModelFeatures m_Features;

		// storage for HasData classes
		Storage< Real, TimeInSeconds > m_Data;
		bool m_StoreData;
		TimeInSeconds m_StoreDataInterval;
		StoreDataFlags m_StoreDataFlags;
	};
}
