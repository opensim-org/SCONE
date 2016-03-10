#include "stdafx.h"
#include "Model.h"

#include "Body.h"
#include "Joint.h"
#include "Dof.h"
#include "Muscle.h"
#include "boost/foreach.hpp"
#include <algorithm>

#include "../core/Profiler.h"
#include "SensorDelayAdapter.h"
#include "../core/InitFromPropNode.h"
#include "Factories.h"

using std::endl;

namespace scone
{
	namespace sim
	{
		Model::Model( const PropNode& props, opt::ParamSet& par ) :
		HasSignature( props ),
		m_ShouldTerminate( false ),
		custom_properties( props.TryGetChild( "custom_properties" ) ),
		m_OriSensors(),
		m_StoreData( false )
		{
			INIT_PROPERTY( props, sensor_delay_scaling_factor, 1.0 );
			INIT_PARAM( props, par, balance_sensor_delay, 0.0 );
			INIT_PARAM( props, par, balance_sensor_ori_vel_gain, 0.0 );

			log::DebugF( "Orientation sensors initial values: %p %p %p", m_OriSensors[ 0 ], m_OriSensors[ 1 ], m_OriSensors[ 2 ] );
		}

		Model::~Model()
		{
		}

		std::ostream& Model::ToStream( std::ostream& str ) const
		{
			str << "Links:" << endl;
			str << GetRootLink().ToString();

			return str;
		}

		Sensor& Model::AcquireSensor( const PropNode& pn, opt::ParamSet& par, const sim::Area& area )
		{
			// create the sensor first, so we can use its name to find it
			SensorUP sensor = sim::CreateSensor( pn, par, *this, area );

			// see if there's a sensor with the same name (should be unique)
			auto it = std::find_if( m_Sensors.begin(), m_Sensors.end(), [&]( SensorUP& s ) { return s->GetName() == sensor->GetName(); } );

			if ( it == m_Sensors.end() )
			{
				// add the new sensor and return it
				m_Sensors.push_back( std::move( sensor ) );
				return *m_Sensors.back(); // return newly added sensor
			}
			else return **it; // return found element (sensor gets deleted)
		}

		SensorDelayAdapter& Model::AcquireSensorDelayAdapter( Sensor& source )
		{
			auto it = std::find_if( m_SensorDelayAdapters.begin(), m_SensorDelayAdapters.end(),
				[&]( SensorDelayAdapterUP& a ) { return &a->GetInputSensor() == &source; } );

			if ( it == m_SensorDelayAdapters.end() )
			{
				m_SensorDelayAdapters.push_back( SensorDelayAdapterUP( new SensorDelayAdapter( *this, source, 0.0 ) ) );
				return *m_SensorDelayAdapters.back();
			}
			else return **it;
		}

		SensorDelayAdapter& Model::AcquireDelayedSensor( const PropNode& pn, opt::ParamSet& par, const sim::Area& area )
		{
			// acquire sensor first
			return AcquireSensorDelayAdapter( AcquireSensor( pn, par, area ) );
		}

		Vec3 Model::GetDelayedOrientation()
		{
			SCONE_ASSERT( m_OriSensors[ 0 ] );
			return Vec3( m_OriSensors[ 0 ]->GetValue( balance_sensor_delay ),
				m_OriSensors[ 1 ]->GetValue( balance_sensor_delay ),
				m_OriSensors[ 2 ]->GetValue( balance_sensor_delay ) );
		}

		void Model::UpdateSensorDelayAdapters()
		{
			SCONE_PROFILE_SCOPE;
			//SCONE_THROW_IF( GetIntegrationStep() != GetPreviousIntegrationStep() + 1, "SensorDelayAdapters should only be updated at each new integration step" );
			SCONE_ASSERT( m_SensorDelayStorage.IsEmpty() || GetPreviousTime() == m_SensorDelayStorage.Back().GetTime() );

			// add a new frame and update
			m_SensorDelayStorage.AddFrame( GetTime() );
			for ( std::unique_ptr< SensorDelayAdapter >& sda: m_SensorDelayAdapters )
				sda->UpdateStorage();

			//log::TraceF( "Updated Sensor Delays for Int=%03d time=%.6f prev_time=%.6f", GetIntegrationStep(), GetTime(), GetPreviousTime() );
		}

		void Model::CreateBalanceSensors( const PropNode& props, opt::ParamSet& par )
		{
			Real kp = 1;
			Real kd = balance_sensor_ori_vel_gain;

			m_OriSensors[ 0 ] = &AcquireDelayedSensor< OrientationSensor >( *this, OrientationSensor::Coronal, kp, kd );
			m_OriSensors[ 1 ] = &AcquireDelayedSensor< OrientationSensor >( *this, OrientationSensor::Transverse, kp, kd );
			m_OriSensors[ 2 ] = &AcquireDelayedSensor< OrientationSensor >( *this, OrientationSensor::Sagittal, kp, kd );
		}

		void Model::StoreData( Storage< Real >::Frame& frame )
		{
			SCONE_PROFILE_SCOPE;

			// store states
			auto state_values = GetStateValues();
			auto state_names = GetStateVariableNames();

			for ( size_t i = 0; i < state_values.size(); ++i )
				frame[ state_names[ i ] ] = state_values[ i ];

			// store muscle data
			for ( MuscleUP& m : GetMuscles() )
				m->StoreData( frame );

			// store controller data
			for ( ControllerUP& c : GetControllers() )
				c->StoreData( frame );

			// store COP data
			auto cop = GetComPos();
			auto cop_u = GetComVel();
			frame[ "cop_x" ] = cop.x;
			frame[ "cop_y" ] = cop.y;
			frame[ "cop_z" ] = cop.z;
			frame[ "cop_x_u" ] = cop_u.x;
			frame[ "cop_y_u" ] = cop_u.y;
			frame[ "cop_z_u" ] = cop_u.z;
		}

		void Model::StoreCurrentFrame()
		{
			m_Data.AddFrame( GetTime() );
			StoreData( m_Data.Back() );
		}

		void Model::UpdateControlValues()
		{
			SCONE_PROFILE_SCOPE;

			// reset actuator values
			// TODO: not only muscles!
			for ( MuscleUP& mus: GetMuscles() )
				mus->ResetControlValue();

			// update all controllers
			bool terminate = false;
			for ( ControllerUP& con: GetControllers() )
				terminate |= con->UpdateControls( *this, GetTime() ) == Controller::RequestTermination;

			//log::TraceF( "Controls updated for Int=%03d time=%.6f", GetIntegrationStep(), GetTime() );

			if ( terminate )
				SetTerminationRequest();
		}

		void Model::UpdateAnalyses()
		{
			SCONE_PROFILE_SCOPE;

			bool terminate = false;
			for ( ControllerUP& con: GetControllers() )
				terminate |= con->UpdateAnalysis( *this, GetTime() ) == sim::Controller::RequestTermination;

			if ( terminate )
				SetTerminationRequest();
		}

		const Link& Model::FindLink( const String& body_name )
		{
			const Link* link = GetRootLink().FindLink( body_name );
			SCONE_THROW_IF( link == nullptr, "Could not find link " + body_name );
			return *link;
		}

		scone::Real Model::GetTotalContactForce() const
		{
			Real force = 0.0;
			for ( const LegUP& leg: GetLegs() )
				force += leg->GetContactForce().GetLength();
			return force;
		}
	}
}
