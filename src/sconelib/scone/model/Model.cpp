#include "Model.h"

#include "Body.h"
#include "Joint.h"
#include "Dof.h"
#include "Muscle.h"
#include <algorithm>

#include "scone/core/Profiler.h"
#include "scone/core/Log.h"
#include "scone/core/Factories.h"

#include "SensorDelayAdapter.h"
#include "scone/model/State.h"
#include "scone/measures/Measure.h"

#include "xo/container/container_tools.h"
#include "xo/string/string_tools.h"

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
	m_StoreDataFlags( { StoreDataTypes::State, StoreDataTypes::MuscleExcitation, StoreDataTypes::GroundReactionForce, StoreDataTypes::CenterOfMass } ),
	thread_safe_simulation( false ),
	m_Measure( nullptr )
	{
		INIT_PROP( props, sensor_delay_scaling_factor, 1.0 );
		INIT_PAR( props, par, balance_sensor_delay, 0.0 );
		INIT_PAR( props, par, balance_sensor_ori_vel_gain, 0.0 );
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

	Sensor& Model::AcquireSensor( const PropNode& pn, Params& par, const Location& loc )
	{
		// create the sensor first, so we can use its name to find it
		SensorUP sensor = CreateSensor( pn, par, *this, loc );

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

	SensorDelayAdapter& Model::AcquireDelayedSensor( const PropNode& pn, Params& par, const Location& loc )
	{
		// acquire sensor first
		return AcquireSensorDelayAdapter( AcquireSensor( pn, par, loc ) );
	}

	Vec3 Model::GetDelayedOrientation()
	{
		SCONE_ASSERT( m_OriSensors[ 0 ] );
		return Vec3( m_OriSensors[ 0 ]->GetValue( balance_sensor_delay ),
			m_OriSensors[ 1 ]->GetValue( balance_sensor_delay ),
			m_OriSensors[ 2 ]->GetValue( balance_sensor_delay ) );
	}

	String Model::GetClassSignature() const
	{
		auto sig = GetName();
		for ( auto& c : GetControllers() )
		{
			auto controller_sig = c->GetSignature();
			if ( !xo::str_ends_with( sig, "." ) && !xo::str_begins_with( controller_sig, "." ) )
				sig += "."; // add dot if it isn't there
			sig += c->GetSignature();
		}
		return sig;
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

	void Model::SetMeasure( MeasureUP m )
	{
		SCONE_THROW_IF( m_Measure, "Only one measure allowed" );
		m_Measure = m.get();
		m_Controllers.push_back( std::move( m ) );
	}

	void Model::CreateControllers( const PropNode& pn, Params& par )
	{
		SCONE_PROFILE_FUNCTION;

		// add controller (new style)
		if ( auto* cprops = pn.try_get_child( "Controller" ) )
			m_Controllers.push_back( CreateController( *cprops, par, *this, Location( NoSide ) ) );

		// add measure (new style)
		if ( auto* cprops = pn.try_get_child( "Measure" ) )
			SetMeasure( CreateMeasure( *cprops, par, *this, Location( NoSide ) ) );

		// add multiple controllers / measures (old style)
		if ( auto* cprops = pn.try_get_child( "Controllers" ) )
		{
			for ( auto it = cprops->begin(); it != cprops->end(); ++it )
			{
				if ( it->first == "Controller" )
					m_Controllers.push_back( CreateController( it->second, par, *this, Location( NoSide ) ) );
				else if ( it->first == "Measure" )
					SetMeasure( CreateMeasure( it->second, par, *this, Location( NoSide ) ) );
				else xo_error( "Unknown type " + it->first );
			}
		}
	}

	bool Model::GetStoreData() const
	{
		return m_StoreData && ( m_Data.IsEmpty() || xo::greater_or_equal( GetTime() - m_Data.Back().GetTime(), m_StoreDataInterval, 1e-6 ) );
	}

	void Model::StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags ) const
	{
		SCONE_PROFILE_FUNCTION;

		// store states
		if ( flags( StoreDataTypes::State ) )
		{
			for ( size_t i = 0; i < GetState().GetSize(); ++i )
				frame[ GetState().GetName( i ) ] = GetState().GetValue( i );
		}

		// store muscle data
		for ( auto& m : GetMuscles() )
			m->StoreData( frame, flags );

		// store body data
		for ( auto& b : GetBodies() )
			b->StoreData( frame, flags );

		// store joint reaction force magnitude
		if ( flags( StoreDataTypes::JointReactionForce ) )
		{
			for ( auto& joint : GetJoints() )
				frame[ joint->GetName() + ".load" ] = joint->GetLoad();
		}

		// store dof data
		if ( flags( StoreDataTypes::JointMoment ) )
		{
			for ( auto& d : GetDofs() )
				frame[ d->GetName() + ".moment" ] = d->GetMoment();
		}

		// store controller data
		if ( flags( StoreDataTypes::ControllerData ) )
		{
			for ( auto& c : GetControllers() )
				c->StoreData( frame, flags );
		}

		// store sensor data
		if ( flags( StoreDataTypes::SensorData ) )
		{
			auto sf = m_SensorDelayStorage.Back();
			for ( index_t i = 0; i < m_SensorDelayStorage.GetChannelCount(); ++i )
				frame[ m_SensorDelayStorage.GetLabels()[ i ] ] = sf[ i ];
		}

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
	}

	void Model::StoreCurrentFrame()
	{
		if ( m_Data.IsEmpty() || GetTime() > m_Data.Back().GetTime() )
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
			terminate |= con->UpdateControls( *this, GetTime() ) == true;

		//log::TraceF( "Controls updated for Int=%03d time=%.6f", GetIntegrationStep(), GetTime() );

		if ( terminate )
			SetTerminationRequest();
	}

	void Model::UpdateAnalyses()
	{
		SCONE_PROFILE_FUNCTION;

		bool terminate = false;
		for ( ControllerUP& con : GetControllers() )
			terminate |= con->UpdateAnalysis( *this, GetTime() ) == true;

		if ( terminate )
			SetTerminationRequest();
	}

	const Link& Model::FindLink( const String& body_name )
	{
		const Link* link = GetRootLink().FindLink( body_name );
		SCONE_THROW_IF( link == nullptr, "Could not find link " + body_name );
		return *link;
	}

	void Model::SetNullState()
	{
		State zero_state = GetState();
		for ( index_t i = 0; i < zero_state.GetSize(); ++i )
		{
			if ( !xo::str_ends_with( zero_state.GetName( i ), ".fiber_length" ) &&
				 !xo::str_ends_with( zero_state.GetName( i ), ".activation" ) )
				zero_state.SetValue( i, 0 );
		}
		SetState( zero_state, 0 );
	}

	void Model::SetNeutralState()
	{
		for ( auto& dof : GetDofs() )
		{
			dof->SetPos( dof->GetRange().GetCenter() );
			dof->SetVel( 0 );
		}
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
