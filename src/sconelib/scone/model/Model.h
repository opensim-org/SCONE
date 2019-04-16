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

#include "scone/controllers/Controller.h"
#include "Link.h"
#include "Leg.h"

#include <vector>
#include "scone/core/HasName.h"
#include "scone/core/HasSignature.h"
#include "scone/core/HasExternalResources.h"
#include "Sensor.h"
#include "scone/core/Storage.h"
#include <array>
#include <type_traits>

#include "ContactGeometry.h"
#include "scone/measures/Measure.h"
#include "scone/controllers/Controller.h"

namespace scone
{
	/// Simulation model.
	class SCONE_API Model : public HasName, public HasSignature, public HasData, public HasExternalResources
	{
	public:
		Model( const PropNode& props, Params& par );
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

		/// Actuator access
		std::vector< Actuator* >& GetActuators() { return m_Actuators; }

		/// Contact geometries
		std::vector< ContactGeometry > GetContactGeometries() { return m_ContactGeometries; }

		/// link access
		const Link& FindLink( const String& body_name );
		const Link& GetRootLink() const { return *m_RootLink; }

		/// Controller access
		Controller* GetController() { return m_Controller.get(); }
		const Controller* GetController() const { return m_Controller.get(); }
		virtual void SetController( ControllerUP c ) { SCONE_ASSERT( !m_Controller ); m_Controller = std::move( c ); }

		/// Measure access
		Measure* GetMeasure() { return m_Measure.get(); }
		const Measure* GetMeasure() const { return m_Measure.get(); }
		void SetMeasure( MeasureUP m ) { SCONE_ASSERT( !m_Measure ); m_Measure = std::move( m ); }

		void UpdateControlValues();
		void UpdateAnalyses();

		/// leg access
		size_t GetLegCount() const { return m_Legs.size(); }
		const Leg& GetLeg( size_t idx ) const { return *m_Legs[ idx ]; }
		const Leg& GetLeg( const Location& loc ) const { for ( auto& l : m_Legs ) if ( l->GetSide() == loc.GetSide() ) return *l; xo_error( "Could not find leg" ); }
		std::vector< LegUP >& GetLegs() { return m_Legs; }
		const std::vector< LegUP >& GetLegs() const { return m_Legs; }

		/// Get simulation info
		virtual TimeInSeconds GetTime() const = 0;
		virtual int GetIntegrationStep() const = 0;
		virtual int GetPreviousIntegrationStep() const = 0;
		virtual TimeInSeconds GetPreviousTime() const = 0;
		virtual TimeInSeconds GetDeltaTime() const { return GetTime() - GetPreviousTime(); }
		virtual TimeInSeconds GetSimulationStepSize() = 0;

		/// Model state access
		virtual const State& GetState() const = 0;
		virtual State& GetState() = 0;
		virtual void SetState( const State& state, TimeInSeconds timestamp ) = 0;
		virtual void SetStateValues( const std::vector< Real >& state, TimeInSeconds timestamp ) = 0;
		void SetNullState();
		void SetNeutralState();

		/// Simulate model
		virtual void AdvanceSimulationTo( double time ) = 0;
		virtual double GetSimulationEndTime() const = 0;
		virtual void SetSimulationEndTime( double time ) = 0;
		virtual bool HasSimulationEnded() { return m_ShouldTerminate || GetTime() >= GetSimulationEndTime(); }
		virtual void RequestTermination() { m_ShouldTerminate = true; }

		/// Model data
		virtual const Storage< Real, TimeInSeconds > GetData() { return m_Data; }
		virtual std::vector<path> WriteResults( const path& file_base ) const = 0;

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
		const PropNode* GetCustomProps() { return m_pCustomProps; }
		const PropNode* GetModelProps() { return m_pModelProps; }
		PropNode& GetUserData() { return m_UserData; }

		// streaming operator (for debugging)
		virtual std::ostream& ToStream( std::ostream& str ) const;

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

		/// Pattern matching the states to include in initial offset (comma seperated); default = "*".
		String initial_state_offset_include;

		/// Pattern matching the states to exclude in initial offset (comma seperated); default = "".
		String initial_state_offset_exclude;

		/// Maximum integration step size; default = 0.001.
		double max_step_size;

		/// Use fixed step size for controllers; default = true.
		bool use_fixed_control_step_size;

		/// Step size used for controllers; default = 0.001.
		double fixed_control_step_size;

		/// Initial load [BW] at which to place the model initially; default = 0.2;
		Real initial_load;

		/// Scaling factor to apply to all sensor delays; default = 1.
		Real sensor_delay_scaling_factor;

		void SetStoreData( bool store ) { m_StoreData = store; }
		bool GetStoreData() const;
		StoreDataFlags& GetStoreDataFlags() { return m_StoreDataFlags; }
		const StoreDataFlags& GetStoreDataFlags() const { return m_StoreDataFlags; }

	protected:
		virtual String GetClassSignature() const override;
		void UpdateSensorDelayAdapters();
		void CreateControllers( const PropNode& pn, Params& par );

		virtual void StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags ) const override;
		virtual void StoreCurrentFrame();

	protected:
		LinkUP m_RootLink;
		std::vector< MuscleUP > m_Muscles;
		std::vector< BodyUP > m_Bodies;
		std::vector< JointUP > m_Joints;
		std::vector< DofUP > m_Dofs;
		std::vector< LegUP > m_Legs;
		std::vector< ContactGeometry > m_ContactGeometries;
		bool m_ShouldTerminate;

		MeasureUP m_Measure;
		ControllerUP m_Controller;

		// non-owning storage
		std::vector< Actuator* > m_Actuators;
		Storage< Real > m_SensorDelayStorage;
		std::vector< std::unique_ptr< SensorDelayAdapter > > m_SensorDelayAdapters;
		std::vector< std::unique_ptr< Sensor > > m_Sensors;

		const PropNode* m_pModelProps;
		const PropNode* m_pCustomProps;
		PropNode m_UserData;

		// storage for HasData classes
		Storage< Real, TimeInSeconds > m_Data;
		bool m_StoreData;
		TimeInSeconds m_StoreDataInterval;
		StoreDataFlags m_StoreDataFlags;
	};

	inline std::ostream& operator<<( std::ostream& str, const Model& model ) { return model.ToStream( str ); }
}
