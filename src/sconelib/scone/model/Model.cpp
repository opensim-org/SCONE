/*
** Model.cpp
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "Model.h"
#include "Body.h"
#include "Dof.h"
#include "Joint.h"
#include "Muscle.h"
#include "SensorDelayAdapter.h"
#include "State.h"
#include "scone/controllers/CompositeController.h"
#include "scone/core/Factories.h"
#include "scone/core/Log.h"
#include "scone/core/profiler_config.h"
#include "scone/core/Settings.h"
#include "scone/measures/Measure.h"
#include "xo/container/container_tools.h"
#include "xo/string/string_tools.h"

#include <algorithm>
#include <tuple>

using std::endl;

namespace scone
{
	Model::Model( const PropNode& props, Params& par ) :
		HasSignature( props ),
		m_Measure( nullptr ),
		m_Controller( nullptr ),
		m_ShouldTerminate( false ),
		m_pModelProps( props.try_get_child( "ModelProperties" ) ),
		m_pCustomProps( props.try_get_child( "CustomProperties" ) ),
		m_StoreData( false ),
		m_StoreDataFlags( { StoreDataTypes::State, StoreDataTypes::ActuatorInput, StoreDataTypes::MuscleExcitation, StoreDataTypes::GroundReactionForce, StoreDataTypes::ContactForce, StoreDataTypes::CenterOfMass } )
	{
		// old-style initialization (for backwards compatibility)
		if ( auto sio = props.try_get_child( "state_init_optimization" ) )
		{
			initial_state_offset = sio->try_get_child( "offset" );
			initial_state_offset_symmetric = sio->get( "symmetric", false );
			initial_state_offset_include = sio->get< String >( "include_states", "*" );
			initial_state_offset_exclude = sio->get< String >( "exclude_states", "" );
		}
		else
		{
			initial_state_offset = props.try_get_child( "initial_state_offset" );
			INIT_PROP( props, initial_state_offset_symmetric, false );
			INIT_PROP( props, initial_state_offset_include, "*" );
			INIT_PROP( props, initial_state_offset_exclude, "" );
		}

		INIT_PROP( props, max_step_size, 0.001 );
		INIT_PROP( props, fixed_control_step_size, 0.001 );
		INIT_PROP( props, use_fixed_control_step_size, fixed_control_step_size > 0 );
		INIT_PROP( props, initial_load, 0.2 );
		INIT_PROP( props, sensor_delay_scaling_factor, 1.0 );

		// set store data info from settings
		m_StoreDataInterval = 1.0 / GetSconeSettings().get< double >( "data.frequency" );
		auto& flags = GetStoreDataFlags();
		flags.set( { StoreDataTypes::MuscleExcitation, StoreDataTypes::MuscleFiberProperties }, GetSconeSettings().get< bool >( "data.muscle" ) );
		flags.set( { StoreDataTypes::BodyComPosition, StoreDataTypes::BodyOrientation }, GetSconeSettings().get< bool >( "data.body" ) );
		flags.set( StoreDataTypes::JointReactionForce, GetSconeSettings().get< bool >( "data.joint" ) );
		flags.set( StoreDataTypes::SensorData, GetSconeSettings().get< bool >( "data.sensor" ) );
		flags.set( StoreDataTypes::ControllerData, GetSconeSettings().get< bool >( "data.controller" ) );
		flags.set( StoreDataTypes::ContactForce, GetSconeSettings().get< bool >( "data.contact" ) );
	}

	Model::~Model()
	{
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

	String Model::GetClassSignature() const
	{
		auto sig = GetName();
		if ( GetController() )
			sig += '.' + GetController()->GetSignature();
		if ( GetMeasure() )
			sig += '.' + GetMeasure()->GetSignature();
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

	void Model::CreateControllers( const PropNode& pn, Params& par )
	{
		// add controller (new style, prefer define outside model)
		if ( auto* cprops = pn.try_get_child( "Controller" ) )
			SetController( CreateController( *cprops, par, *this, Location() ) );

		// add measure (new style, prefer define outside model)
		if ( auto* cprops = pn.try_get_child( "Measure" ) )
			SetMeasure( CreateMeasure( *cprops, par, *this, Location() ) );

		// add multiple controllers / measures (old style)
		if ( auto* cprops = pn.try_get_child( "Controllers" ) )
		{
			SetController( std::make_unique< CompositeController >( *cprops, par, *this, Location() ) );
			if ( auto* mprops = cprops->try_get_child( "Measure" ) )
				SetMeasure( CreateMeasure( *mprops, par, *this, Location() ) );
		}
	}

	bool Model::GetStoreData() const
	{
		return m_StoreData && ( m_Data.IsEmpty() || xo::greater_than_or_equal( GetTime() - m_Data.Back().GetTime(), m_StoreDataInterval, 1e-6 ) );
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

		// store actuator data
		for ( auto& m : GetActuators() )
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
		if ( flags( StoreDataTypes::DofMoment ) )
		{
			for ( auto& d : GetDofs() )
				frame[ d->GetName() + ".moment" ] = d->GetMoment();
		}

		// store controller data
		if ( flags( StoreDataTypes::ControllerData ) )
		{
			if ( GetController() ) GetController()->StoreData( frame, flags );
			if ( GetMeasure() ) GetMeasure()->StoreData( frame, flags );
		}

		// store sensor data
		if ( flags( StoreDataTypes::SensorData ) && !m_SensorDelayStorage.IsEmpty() )
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
				Vec3 force, moment, cop;
				leg->GetContactForceMomentCop( force, moment, cop );
				Vec3 grf = force / GetBW();

				frame[ leg->GetName() + ".grf_norm_x" ] = grf.x;
				frame[ leg->GetName() + ".grf_norm_y" ] = grf.y;
				frame[ leg->GetName() + ".grf_norm_z" ] = grf.z;
				frame[ leg->GetName() + ".grf_x" ] = force.x;
				frame[ leg->GetName() + ".grf_y" ] = force.y;
				frame[ leg->GetName() + ".grf_z" ] = force.z;
				frame[ leg->GetName() + ".grm_x" ] = moment.x;
				frame[ leg->GetName() + ".grm_y" ] = moment.y;
				frame[ leg->GetName() + ".grm_z" ] = moment.z;
				frame[ leg->GetName() + ".cop_x" ] = cop.x;
				frame[ leg->GetName() + ".cop_y" ] = cop.y;
				frame[ leg->GetName() + ".cop_z" ] = cop.z;
			}
		}

		if ( flags( StoreDataTypes::ContactForce ) )
		{
			for ( auto& force : GetContactForces() )
				force->StoreData( frame, flags );
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
		for ( Actuator* a : GetActuators() )
			a->ClearInput();

		// update all controllers
		bool terminate = false;
		terminate |= GetController()->UpdateControls( *this, GetTime() );

		if ( terminate )
			RequestTermination();
	}

	void Model::UpdateAnalyses()
	{
		SCONE_PROFILE_FUNCTION;

		bool terminate = false;
		terminate |= GetController()->UpdateAnalysis( *this, GetTime() );
		terminate |= GetMeasure()->UpdateAnalysis( *this, GetTime() );

		if ( terminate )
			RequestTermination();
	}

	std::vector< ForceValue > Model::GetContactForceValues() const
	{
		std::vector< ForceValue > fvec;
		fvec.reserve( GetContactForces().size() );
		for ( auto& cf : GetContactForces() )
		{
			auto cfv = cf->GetForceValue();
			if ( xo::squared_length( cfv.force ) > REAL_WIDE_EPSILON )
				fvec.push_back( cf->GetForceValue() );
		}
		return fvec;
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

	Real Model::GetComHeight( const Vec3& up ) const
	{
		auto com = GetComPos();
		if ( auto* ground = GetGroundPlane() )
		{
			auto r = xo::linef( xo::vec3f( com ), xo::vec3f( -up ) );
			auto t = xo::transformf( xo::vec3f( ground->GetPos() ), xo::quatf( ground->GetOri() ) );
			auto p = xo::intersection( r, std::get<xo::plane>( ground->GetShape() ), t );
			return com.y - p.y;
		}
		else return com.y;
	}

	Real Model::GetTotalContactForce() const
	{
		Real force = 0.0;
		for ( const LegUP& leg : GetLegs() )
			force += xo::length( leg->GetContactForce() );
		return force;
	}

	Real Model::GetBW() const
	{
		return GetMass() * xo::length( GetGravity() );
	}

	const ContactGeometry* Model::GetGroundPlane() const
	{
		auto& cg = GetContactGeometries();
		if ( cg.size() > 0 && std::holds_alternative< xo::plane >( cg.front()->GetShape() ) )
			return cg.front().get();
		else return nullptr;
	}
}
