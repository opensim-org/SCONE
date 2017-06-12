#include "Model.h"

#include "Body.h"
#include "Joint.h"
#include "Dof.h"
#include "Muscle.h"
#include <algorithm>

#include "scone/core/Profiler.h"
#include "scone/core/Log.h"
#include "scone/core/propnode_tools.h"
#include "scone/core/Factories.h"

#include "SensorDelayAdapter.h"
#include "scone/model/State.h"

using std::endl;

namespace scone
{
	Model::Model( const PropNode& props, Params& par ) :
		HasSignature( props ),
		m_ShouldTerminate( false ),
		m_pCustomProps( props.try_get_child( "CustomProperties" ) ),
		m_pModelProps( props.try_get_child( "ModelProperties" ) ),
		m_OriSensors(),
		m_StoreData( false ),
		m_StoreDataFlags( { StoreDataTypes::State, StoreDataTypes::MuscleExcitation, StoreDataTypes::GroundReactionForce, StoreDataTypes::CenterOfMass, StoreDataTypes::ControllerData } ),
		thread_safe_simulation( false )
	{
		INIT_PROPERTY( props, sensor_delay_scaling_factor, 1.0 );
		INIT_PARAM( props, par, balance_sensor_delay, 0.0 );
		INIT_PARAM( props, par, balance_sensor_ori_vel_gain, 0.0 );
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

	Sensor& Model::AcquireSensor( const PropNode& pn, Params& par, const Locality& area )
	{
		// create the sensor first, so we can use its name to find it
		SensorUP sensor = CreateSensor( pn, par, *this, area );

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

	SensorDelayAdapter& Model::AcquireDelayedSensor( const PropNode& pn, Params& par, const Locality& area )
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
		SCONE_PROFILE_FUNCTION;
		//SCONE_THROW_IF( GetIntegrationStep() != GetPreviousIntegrationStep() + 1, "SensorDelayAdapters should only be updated at each new integration step" );
		SCONE_ASSERT( m_SensorDelayStorage.IsEmpty() || GetPreviousTime() == m_SensorDelayStorage.Back().GetTime() );

		// add a new frame and update
		m_SensorDelayStorage.AddFrame( GetTime() );
		for ( std::unique_ptr< SensorDelayAdapter >& sda : m_SensorDelayAdapters )
			sda->UpdateStorage();

		//log::TraceF( "Updated Sensor Delays for Int=%03d time=%.6f prev_time=%.6f", GetIntegrationStep(), GetTime(), GetPreviousTime() );
	}

	void Model::CreateBalanceSensors( const PropNode& props, Params& par )
	{
		Real kp = 1;
		Real kd = balance_sensor_ori_vel_gain;

		m_OriSensors[ 0 ] = &AcquireDelayedSensor< OrientationSensor >( *this, OrientationSensor::Coronal, kp, kd );
		m_OriSensors[ 1 ] = &AcquireDelayedSensor< OrientationSensor >( *this, OrientationSensor::Transverse, kp, kd );
		m_OriSensors[ 2 ] = &AcquireDelayedSensor< OrientationSensor >( *this, OrientationSensor::Sagittal, kp, kd );
	}

	void Model::StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags )
	{
		SCONE_PROFILE_FUNCTION;

		// store states
		if ( flags( StoreDataTypes::State ) )
		{
			for ( size_t i = 0; i < GetState().GetSize(); ++i )
				frame[ GetState().GetName( i ) ] = GetState().GetValue( i );
		}

		// store muscle data
		for ( MuscleUP& m : GetMuscles() )
			m->StoreData( frame, flags );

		// store COP data
		if ( flags( StoreDataTypes::CenterOfMass ) )
		{
			auto com = GetComPos();
			auto com_u = GetComVel();
			frame[ "com_x" ] = com.x;
			frame[ "com_y" ] = com.y;
			frame[ "com_z" ] = com.z;
			frame[ "com_x_u" ] = com_u.x;
			frame[ "com_y_u" ] = com_u.y;
			frame[ "com_z_u" ] = com_u.z;
		}

		// store GRF data (measured in BW)
		if ( flags( StoreDataTypes::GroundReactionForce ) )
		{
			for ( auto& leg : GetLegs() )
			{
				auto grf = leg->GetContactForce() / GetBW();
				frame[ leg->GetName() + ".grf_x" ] = grf.x;
				frame[ leg->GetName() + ".grf_y" ] = grf.y;
				frame[ leg->GetName() + ".grf_z" ] = grf.z;
			}

		}

		// store joint reaction force magnitude
		if ( flags( StoreDataTypes::JointReactionForce ) )
		{
			for ( auto& joint : GetJoints() )
				frame[ joint->GetName() + ".jrf" ] = joint->GetLoad();
		}

		// store controller data
		if ( flags( StoreDataTypes::ControllerData ) )
		{
			for ( ControllerUP& c : GetControllers() )
				c->StoreData( frame, flags );
		}
	}

	void Model::StoreCurrentFrame()
	{
		m_Data.AddFrame( GetTime() );
		StoreData( m_Data.Back(), m_StoreDataFlags );
	}

	void Model::UpdateControlValues()
	{
		SCONE_PROFILE_FUNCTION;

		// reset actuator values
		// TODO: not only muscles!
		for ( MuscleUP& mus : GetMuscles() )
			mus->ClearInput();

		// update all controllers
		bool terminate = false;
		for ( ControllerUP& con : GetControllers() )
			terminate |= con->UpdateControls( *this, GetTime() ) == Controller::RequestTermination;

		//log::TraceF( "Controls updated for Int=%03d time=%.6f", GetIntegrationStep(), GetTime() );

		if ( terminate )
			SetTerminationRequest();
	}

	void Model::UpdateAnalyses()
	{
		SCONE_PROFILE_FUNCTION;

		bool terminate = false;
		for ( ControllerUP& con : GetControllers() )
			terminate |= con->UpdateAnalysis( *this, GetTime() ) == Controller::RequestTermination;

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
		for ( const LegUP& leg : GetLegs() )
			force += leg->GetContactForce().length();
		return force;
	}

	scone::Real Model::GetBW() const
	{
		return GetMass() * GetGravity().length();
	}
}
