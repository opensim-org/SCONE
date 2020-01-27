/*
** ModelOpenSim3.cpp
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "scone/core/Exception.h"
#include "scone/core/Log.h"
#include "scone/core/Factories.h"
#include "scone/core/StorageIo.h"

#include "ModelOpenSim3.h"
#include "BodyOpenSim3.h"
#include "MuscleOpenSim3.h"
#include "SimulationOpenSim3.h"
#include "JointOpenSim3.h"
#include "DofOpenSim3.h"
#include "ConstantForce.h"
#include "ContactForceOpenSim3.h"
#include "simbody_tools.h"

#include <OpenSim/OpenSim.h>
#include <OpenSim/Simulation/Model/Umberger2010MuscleMetabolicsProbe.h>
#include <OpenSim/Simulation/Model/Bhargava2004MuscleMetabolicsProbe.h>

#include "scone/core/system_tools.h"
#include "scone/core/profiler_config.h"

#include "xo/string/string_tools.h"
#include "xo/string/pattern_matcher.h"
#include "xo/container/container_tools.h"
#include "xo/utility/file_resource_cache.h"
#include "xo/geometry/quat.h"
#include "xo/geometry/angle.h"

#include "spot/par_tools.h"

#include <mutex>

using std::cout;
using std::endl;

namespace scone
{
	std::mutex g_SimBodyMutex;

	xo::file_resource_cache< OpenSim::Model, std::string > g_ModelCache;
	xo::file_resource_cache< OpenSim::Storage, std::string > g_StorageCache;

	// OpenSim3 controller that calls scone controllers
	class ControllerDispatcher : public OpenSim::Controller
	{
	public:
		ControllerDispatcher( ModelOpenSim3& model ) : m_Model( model ) { };
		virtual void computeControls( const SimTK::State& s, SimTK::Vector &controls ) const override;
		virtual ControllerDispatcher* clone() const override { return new ControllerDispatcher( *this ); }
		virtual const std::string& getConcreteClassName() const override { SCONE_THROW_NOT_IMPLEMENTED; }

	private:
		ModelOpenSim3& m_Model;
	};

	// Constructor
	ModelOpenSim3::ModelOpenSim3( const PropNode& props, Params& par ) :
		Model( props, par ),
		m_pOsimModel( nullptr ),
		m_pTkState( nullptr ),
		m_pProbe( 0 ),
		m_pControllerDispatcher( nullptr ),
		m_PrevIntStep( -1 ),
		m_PrevTime( 0.0 ),
		m_Mass( 0.0 ),
		m_BW( 0.0 )
	{
		SCONE_PROFILE_FUNCTION;

		path state_init_file;
		String probe_class;

		INIT_PROP( props, integration_accuracy, 0.001 );
		INIT_PROP( props, integration_method, String( "SemiExplicitEuler2" ) );

		INIT_PROP_REQUIRED( props, model_file );
		INIT_PROP( props, state_init_file, path() );
		INIT_PROP( props, probe_class, String() );

		INIT_PROP( props, initial_load_dof, "pelvis_ty" );
		INIT_PROP( props, enable_external_forces, false );

		INIT_PROP( props, leg_upper_body, "femur" );
		INIT_PROP( props, leg_lower_body, "" );
		INIT_PROP( props, leg_contact_force, "foot" );

		// always set create_body_forces when there's a PerturbationController
		// #todo: think of a nicer, more generic way of dealing with this issue
		if ( auto* controller = props.try_get_child( "Controller" ) )
		{
			for ( auto& cprops : controller->select( "Controller" ) )
				enable_external_forces |= cprops.second.get<string>( "type" ) == "PerturbationController";
		}

		// create new OpenSim Model using resource cache
		{
			SCONE_PROFILE_SCOPE( "CreateModel" );
			model_file = FindFile( model_file );
			m_pOsimModel = g_ModelCache( model_file.str() );
			AddExternalResource( model_file );
		}

		// create torque and point actuators
		if ( enable_external_forces )
		{
			SCONE_PROFILE_SCOPE( "SetupBodyForces" );
			for ( int idx = 0; idx < m_pOsimModel->getBodySet().getSize(); ++idx )
			{
				OpenSim::ConstantForce* cf = new OpenSim::ConstantForce( m_pOsimModel->getBodySet().get( idx ).getName() );
				cf->set_point_is_global( false );
				cf->set_force_is_global( true );
				cf->set_torque_is_global( false );
				m_BodyForces.push_back( cf );
				m_pOsimModel->addForce( cf );
			}
		}

		{
			//SCONE_PROFILE_SCOPE( "SetupOpenSimParameters" );

			// change model properties
			if ( auto* model_pars = props.try_get_child( "Properties" ) )
				SetProperties( *model_pars, par );

			// create controller dispatcher (ownership is automatically passed to OpenSim::Model)
			m_pControllerDispatcher = new ControllerDispatcher( *this );
			m_pOsimModel->addController( m_pControllerDispatcher );

			// create probe (ownership is automatically passed to OpenSim::Model)
			// OpenSim: this doesn't work! It either crashes or gives inconsistent results
			if ( probe_class == "Umberger2010MuscleMetabolicsProbe" )
			{
				auto probe = new OpenSim::Umberger2010MuscleMetabolicsProbe( true, true, true, true );
				GetOsimModel().addProbe( probe );
				for ( int idx = 0; idx < GetOsimModel().getMuscles().getSize(); ++idx )
				{
					OpenSim::Muscle& mus = GetOsimModel().getMuscles().get( idx );
					double mass = ( mus.getMaxIsometricForce() / 0.25e6 ) * 1059.7 * mus.getOptimalFiberLength(); // Derived from OpenSim doxygen
					probe->addMuscle( mus.getName(), 0.5 );
				}
				probe->setInitialConditions( SimTK::Vector( 1, 0.0 ) );
				probe->setOperation( "integrate" );
				m_pProbe = probe;
			}
		}

		// Initialize the system
		// This is not thread-safe in case an exception is thrown, so we add a mutex guard
		{
			SCONE_PROFILE_SCOPE( "InitSystem" );
			std::scoped_lock lock( g_SimBodyMutex );
			m_pTkState = &m_pOsimModel->initSystem();
		}

		// create model component wrappers and sensors
		{
			//SCONE_PROFILE_SCOPE( "CreateWrappers" );
			CreateModelWrappers( props, par );
			AddExternalDisplayGeometries( model_file.parent_path() );
		}

		{
			//SCONE_PROFILE_SCOPE( "InitVariables" );
			// initialize cached variables to save computation time
			m_Mass = m_pOsimModel->getMultibodySystem().getMatterSubsystem().calcSystemMass( m_pOsimModel->getWorkingState() );
			m_BW = xo::length( GetGravity() ) * GetMass();
			ValidateDofAxes();
		}

		// Create the integrator for the simulation.
		{
			//SCONE_PROFILE_SCOPE( "InitIntegrators" );
			if ( integration_method == "RungeKuttaMerson" )
				m_pTkIntegrator = std::unique_ptr< SimTK::Integrator >( new SimTK::RungeKuttaMersonIntegrator( m_pOsimModel->getMultibodySystem() ) );
			else if ( integration_method == "RungeKutta2" )
				m_pTkIntegrator = std::unique_ptr< SimTK::Integrator >( new SimTK::RungeKutta2Integrator( m_pOsimModel->getMultibodySystem() ) );
			else if ( integration_method == "RungeKutta3" )
				m_pTkIntegrator = std::unique_ptr< SimTK::Integrator >( new SimTK::RungeKutta3Integrator( m_pOsimModel->getMultibodySystem() ) );
			else if ( integration_method == "SemiExplicitEuler" )
				m_pTkIntegrator = std::unique_ptr< SimTK::Integrator >( new SimTK::SemiExplicitEulerIntegrator( m_pOsimModel->getMultibodySystem(), max_step_size ) );
			else if ( integration_method == "SemiExplicitEuler2" )
				m_pTkIntegrator = std::unique_ptr< SimTK::Integrator >( new SimTK::SemiExplicitEuler2Integrator( m_pOsimModel->getMultibodySystem() ) );
			else SCONE_THROW( "Invalid integration method: " + xo::quoted( integration_method ) );

			m_pTkIntegrator->setAccuracy( integration_accuracy );
			m_pTkIntegrator->setMaximumStepSize( max_step_size );
			m_pTkIntegrator->resetAllStatistics();
		}

		// read initial state
		{
			SCONE_PROFILE_SCOPE( "InitState" );
			InitStateFromTk();
			if ( !state_init_file.empty() )
			{
				state_init_file = FindFile( state_init_file );
				ReadState( state_init_file );
				AddExternalResource( state_init_file );
			}

			// update state variables if they are being optimized
			if ( initial_state_offset )
			{
				auto inc_pat = xo::pattern_matcher( initial_state_offset_include, ";" );
				auto ex_pat = xo::pattern_matcher( initial_state_offset_exclude + ";*.activation;*.fiber_length", ";" );
				for ( index_t i = 0; i < m_State.GetSize(); ++i )
				{
					const String& state_name = m_State.GetName( i );
					if ( inc_pat( state_name ) && !ex_pat( state_name ) )
					{
						auto par_name = initial_state_offset_symmetric ? GetNameNoSide( state_name ) : state_name;
						m_State[ i ] += par.get( par_name + ".offset", *initial_state_offset );
					}
				}
			}

			// apply and fix state
			if ( !initial_load_dof.empty() && initial_load > 0 && !GetContactGeometries().empty() )
			{
				CopyStateToTk();
				FixTkState( initial_load * GetBW() );
				CopyStateFromTk();
			}
		}

		// Realize acceleration because controllers may need it and in this way the results are consistent
		{
			SCONE_PROFILE_SCOPE( "RealizeSystem" );
			// Create a manager to run the simulation. Can change manager options to save run time and memory or print more information
			m_pOsimManager = std::unique_ptr< OpenSim::Manager >( new OpenSim::Manager( *m_pOsimModel, *m_pTkIntegrator ) );
			m_pOsimManager->setWriteToStorage( false );
			m_pOsimManager->setPerformAnalyses( false );
			m_pOsimManager->setInitialTime( 0.0 );
			m_pOsimManager->setFinalTime( 0.0 );

			m_pOsimModel->getMultibodySystem().realize( GetTkState(), SimTK::Stage::Acceleration );
		}

		// create and initialize controllers
		CreateControllers( props, par );
		log::debug( "Created model ", GetName(), "; dofs=", GetDofs().size(), " muscles=", GetMuscles().size(), " mass=", GetMass() );
	}

	ModelOpenSim3::~ModelOpenSim3() {}

	void ModelOpenSim3::CreateModelWrappers( const PropNode& pn, Params& par )
	{
		SCONE_ASSERT( m_pOsimModel && m_Bodies.empty() && m_Joints.empty() && m_Dofs.empty() && m_Actuators.empty() && m_Muscles.empty() );

		// Create wrappers for bodies
		m_Bodies.reserve( m_pOsimModel->getBodySet().getSize() );
		for ( int idx = 0; idx < m_pOsimModel->getBodySet().getSize(); ++idx )
			m_Bodies.emplace_back( new BodyOpenSim3( *this, m_pOsimModel->getBodySet().get( idx ) ) );

		// Create wrappers for joints
		m_Joints.reserve( m_pOsimModel->getJointSet().getSize() );
		for ( int idx = 0; idx < m_pOsimModel->getJointSet().getSize(); ++idx )
		{
			auto& joint_osim = m_pOsimModel->getJointSet().get( idx );
			auto body_it = xo::find_if( m_Bodies, [&]( BodyUP& body )
				{ return &dynamic_cast<BodyOpenSim3&>( *body ).GetOsBody() == &joint_osim.getBody(); } );
			auto parent_it = xo::find_if( m_Bodies, [&]( BodyUP& body )
				{ return &dynamic_cast<BodyOpenSim3&>( *body ).GetOsBody() == &joint_osim.getParentBody(); } );
			SCONE_ASSERT( body_it != m_Bodies.end() && parent_it != m_Bodies.end() );
			m_Joints.emplace_back( new JointOpenSim3( **body_it, **parent_it, *this, joint_osim ) );
		}

		// create wrappers for dofs
		m_Dofs.reserve( m_pOsimModel->getCoordinateSet().getSize() );
		for ( int idx = 0; idx < m_pOsimModel->getCoordinateSet().getSize(); ++idx )
			m_Dofs.emplace_back( new DofOpenSim3( *this, m_pOsimModel->getCoordinateSet().get( idx ) ) );

		// create contact geometries
		m_ContactGeometries.reserve( m_pOsimModel->getContactGeometrySet().getSize() );
		for ( int idx = 0; idx < m_pOsimModel->getContactGeometrySet().getSize(); ++idx )
		{
			OpenSim::ContactGeometry* cg_osim = &m_pOsimModel->getContactGeometrySet().get( idx );
			const auto& name = cg_osim->getName();
			auto& bod = *FindByName( m_Bodies, cg_osim->getBodyName() );
			auto loc = from_osim( cg_osim->getLocation() );
			auto ea = xo::vec3radd( from_osim( cg_osim->getOrientation() ) );
			auto ori = xo::quat_from_euler( ea, xo::euler_order::xyz );

			if ( auto cs = dynamic_cast< OpenSim::ContactSphere* >( cg_osim ) )
				m_ContactGeometries.emplace_back( new ContactGeometry(
					name, bod, xo::sphere( float( cs->getRadius() ) ), loc, ori ) );
			else if ( auto cp = dynamic_cast< OpenSim::ContactHalfSpace* >( cg_osim ) )
				m_ContactGeometries.emplace_back( new ContactGeometry(
					name, bod, xo::plane( xo::vec3f::neg_unit_x() ), loc, ori ) );
			else if ( auto cm = dynamic_cast<OpenSim::ContactMesh*>( cg_osim ) )
			{
				// #todo: add support for displaying mesh contacts
				auto file = FindFile( model_file.parent_path() / cm->getFilename() );
				AddExternalResource( file );
			}
		}

		// Create wrappers for actuators
		m_Muscles.reserve( m_pOsimModel->getMuscles().getSize() );
		for ( int idx = 0; idx < m_pOsimModel->getActuators().getSize(); ++idx )
		{
			// OpenSim: Set<T>::get( idx ) is const but returns non-const reference, is this a bug?
			OpenSim::Actuator& osAct = m_pOsimModel->getActuators().get( idx );
			if ( OpenSim::Muscle* osMus = dynamic_cast<OpenSim::Muscle*>( &osAct ) )
			{
				m_Muscles.emplace_back( new MuscleOpenSim3( *this, *osMus ) );
				m_Actuators.push_back( m_Muscles.back().get() );
			}
			else if ( auto* osCo = dynamic_cast< OpenSim::CoordinateActuator* >( &osAct ) )
			{
				// add corresponding dof to list of actuators
				auto& dof = dynamic_cast<DofOpenSim3&>( *FindByName( m_Dofs, osCo->getCoordinate()->getName() ) );
				dof.SetCoordinateActuator( osCo );
				m_Actuators.push_back( &dof );
			}
			else
			{
				log::warning( "Unsupported actuator: ", osAct.getName() );
			}
		}

		// Create ContactForce wrappers
		const auto& forces = m_pOsimModel->getForceSet();
		for ( int i = 0; i < forces.getSize(); ++i )
		{
			if ( auto* hcf = dynamic_cast<const OpenSim::HuntCrossleyForce*>( &forces.get( i ) ) )
				m_ContactForces.emplace_back( new ContactForceOpenSim3( *this, *hcf ) );
		}

		// create legs and connect stance_contact forces
		for ( auto side : { LeftSide, RightSide } )
		{
			Body* upper_body = nullptr;
			Body* lower_body = nullptr;
			if ( auto upper_it = TryFindByName( GetBodies(), GetSidedName( leg_upper_body, side ) ); upper_it != GetBodies().end() )
			{
				upper_body = upper_it->get();
				if ( auto lower_it = TryFindByName( GetBodies(), GetSidedName( leg_lower_body, side ) ); lower_it != GetBodies().end() )
					lower_body = lower_it->get();
				else // try finding a body whose grandparent is upper_body (backwards compatibility)
					for ( auto& b : GetBodies() )
						if ( b->GetParentBody() && b->GetParentBody()->GetParentBody() == upper_body )
							lower_body = b.get(); // bingo!
			}
			auto cf_it = TryFindByName( GetContactForces(), GetSidedName( leg_contact_force, side ) );

			if ( upper_body && lower_body && cf_it != GetContactForces().end() )
				m_Legs.emplace_back( new Leg( *upper_body, *lower_body, m_Legs.size(), side, 0, &**cf_it ) );
			//else log::warning( "Could not define leg using ", leg_upper_body, ", ", leg_lower_body, " and ", leg_contact_force );
		}
	}

	template<typename SetT> OpenSim::Object* TryGetOpenSimObject( SetT& set, const String& name ) {
		auto idx = set.getIndex( name );
		return idx != -1 ? &set.get( idx ) : nullptr;
	}

	OpenSim::Object& ModelOpenSim3::FindOpenSimObject( const String& name )
	{
		OpenSim::Object* obj = nullptr;
		if ( obj = TryGetOpenSimObject( m_pOsimModel->updForceSet(), name ) )
			return *obj;
		else if ( obj = TryGetOpenSimObject( m_pOsimModel->updBodySet(), name ) )
			return *obj;
		else if ( obj = TryGetOpenSimObject( m_pOsimModel->updJointSet(), name ) )
			return *obj;
		else if ( obj = TryGetOpenSimObject( m_pOsimModel->updContactGeometrySet(), name ) )
			return *obj;
		else if ( obj = TryGetOpenSimObject( m_pOsimModel->updCoordinateSet(), name ) )
			return *obj;
		SCONE_ERROR( "Could not find OpenSim object " + name );
	}

	void ModelOpenSim3::SetOpenSimObjectProperies( OpenSim::Object& os_object, const PropNode& props, Params& par )
	{
		for ( const auto& [prop_key, prop_val] : props )
		{
			//for ( int i = 0; i < os_object.getNumProperties(); ++i )
			//{
			//	auto& os_prop = os_object.updPropertyByIndex( i );
			//	log::debug( os_object.getName() + "." + os_prop.getName(), " (", os_prop.getTypeName(), ") = ", os_prop.toString() );
			//}

			auto [prop_name, prop_qualifier] = xo::split_str_at_last( prop_key, "." );
			auto& os_prop = os_object.updPropertyByName( prop_name );

			// issue: there doesn't seem to be a way to do this consistently
			// These calls work:
			//  - os_prop.updValue<double>() 
			//  - dynamic_cast<OpenSim::Property<SimTK::Vec3>*>( &os_prop )
			// but this doesn't: 
			//  - os_prop.updValue<SimTK::Vec3>()
			//  - dynamic_cast<OpenSim::Property<double>*>( &os_prop )
			if ( os_prop.getTypeName() == "double" ) // must we do a string check here? :-(
			{
				SCONE_ERROR_IF( prop_val.raw_value().empty(), "Error setting " + os_object.getName() + ": '" + prop_key + "' must have a value" );
				double scenario_value = par.get( prop_key, prop_val );
				if ( prop_qualifier == "factor" )
					os_prop.updValue<double>() *= scenario_value;
				else if ( prop_qualifier.empty() )
					os_prop.updValue<double>() = scenario_value;
				else SCONE_ERROR( "Unsupported qualifier '" + prop_qualifier + "' for " + os_object.getName() + "." + prop_key + "" );
			}
			else if ( auto * vec3_prop = dynamic_cast<OpenSim::Property<SimTK::Vec3>*>( &os_prop ) )
			{
				auto scenario_value = spot::try_get_par( par, prop_key, props, Vec3::zero() );
				if ( prop_qualifier == "offset" )
					vec3_prop->updValue() += to_osim( scenario_value );
				else if ( prop_qualifier.empty() )
					vec3_prop->updValue() = to_osim( scenario_value );
				else SCONE_ERROR( "Unsupported qualifier " + prop_qualifier + " for " + os_object.getName() + "." + prop_key + "" );
			}
			else if ( os_prop.isObjectProperty() )
			{
				log::debug( "Setting Parameter ", os_prop.getName() );
				SetOpenSimObjectProperies( os_prop.updValueAsObject(), prop_val, par );
			}
		}
	}

	void ModelOpenSim3::SetProperties( const PropNode& properties_pn, Params& par )
	{
		for ( const auto& [ object_name, object_props ] : properties_pn )
		{
			ScopedParamSetPrefixer prefix( par, object_name + '.' );
			auto& os_object = FindOpenSimObject( object_name );
			SetOpenSimObjectProperies( os_object, object_props, par );
		}
	}

	Vec3 ModelOpenSim3::GetComPos() const
	{
		return from_osim( m_pOsimModel->calcMassCenterPosition( GetTkState() ) );
	}

	Vec3 ModelOpenSim3::GetComVel() const
	{
		return from_osim( m_pOsimModel->calcMassCenterVelocity( GetTkState() ) );
	}

	Vec3 ModelOpenSim3::GetComAcc() const
	{
		return from_osim( m_pOsimModel->calcMassCenterAcceleration( GetTkState() ) );
	}

	Vec3 ModelOpenSim3::GetGravity() const
	{
		return from_osim( m_pOsimModel->getGravity() );
	}

	void ControllerDispatcher::computeControls( const SimTK::State& s, SimTK::Vector &controls ) const
	{
		// see 'catch' statement below for explanation try {} catch {} is needed
		try
		{
			if ( !m_Model.use_fixed_control_step_size )
			{
				// update current state (#todo: remove const cast)
				m_Model.SetTkState( const_cast<SimTK::State&>( s ) );

				// update SensorDelayAdapters at the beginning of each new step
				// #todo: move this to an analyzer object or some other point
				if ( m_Model.GetIntegrationStep() > m_Model.m_PrevIntStep && m_Model.GetIntegrationStep() > 0 )
				{
					m_Model.UpdateSensorDelayAdapters();
					m_Model.UpdateAnalyses();
				}

				// update actuator values
				m_Model.UpdateControlValues();

				// update previous integration step and time
				// OpenSim: do I need to keep this or is there are smarter way?
				if ( m_Model.GetIntegrationStep() > m_Model.m_PrevIntStep )
				{
					m_Model.m_PrevIntStep = m_Model.GetIntegrationStep();
					m_Model.m_PrevTime = m_Model.GetTime();
				}
			}

			// inject actuator values into controls
			{
				//SCONE_ASSERT_MSG( controls.size() == m_Model.GetMuscles().size(), "Only muscle actuators are supported in SCONE at this moment" );

				int idx = 0;
				for ( auto* act : m_Model.GetActuators() )
				{
					// OpenSim: addInControls is rather inefficient, that's why we don't use it
					controls[ idx++ ] += act->GetInput();
				}
			}
		}
		catch ( std::exception& e )
		{
			// exceptions are caught and reported here
			// otherwise they get lost in SimTK::AbstractIntegratorRep::attemptDAEStep()
			// OpenSim: please remove the catch(...) statement
			log::critical( e.what() );
			throw e;
		}
	}


	void ModelOpenSim3::StoreCurrentFrame()
	{
		SCONE_PROFILE_FUNCTION;

		// store scone data
		Model::StoreCurrentFrame();
	}

	void ModelOpenSim3::AdvanceSimulationTo( double time )
	{
		SCONE_PROFILE_FUNCTION;

		SCONE_ASSERT( m_pOsimManager );

		if ( use_fixed_control_step_size )
		{
			// initialize the time-stepper if this is the first step
			if ( !m_pTkTimeStepper )
			{
				// Integrate using time stepper
				m_pTkTimeStepper = std::unique_ptr< SimTK::TimeStepper >( new SimTK::TimeStepper( m_pOsimModel->getMultibodySystem(), *m_pTkIntegrator ) );
				m_pTkTimeStepper->initialize( GetTkState() );
				if ( GetStoreData() )
				{
					// store initial frame
					m_pOsimModel->getMultibodySystem().realize( GetTkState(), SimTK::Stage::Acceleration );
					CopyStateFromTk();
					StoreCurrentFrame();
				}
			}

			// start integration loop
			int number_of_steps = static_cast<int>( 0.5 + ( time - GetTime() ) / fixed_control_step_size );
			for ( int current_step = 0; current_step < number_of_steps; )
			{
				// update controls
				UpdateControlValues();

				// integrate
				m_PrevTime = GetTime();
				m_PrevIntStep = GetIntegrationStep();
				double target_time = GetTime() + fixed_control_step_size;

				{
					SCONE_PROFILE_SCOPE( "SimTK::TimeStepper::stepTo" );
					auto status = m_pTkTimeStepper->stepTo( target_time );
					if ( status == SimTK::Integrator::EndOfSimulation )
						RequestTermination();
				}

				SetTkState( m_pTkIntegrator->updAdvancedState() );
				CopyStateFromTk();

				++current_step;

				// Realize Acceleration, analysis components may need it
				// this way the results are always consistent
				{
					SCONE_PROFILE_SCOPE( "SimTK::MultibodySystem::realize" );
					m_pOsimModel->getMultibodySystem().realize( GetTkState(), SimTK::Stage::Acceleration );
				}

				// update the sensor delays, analyses, and store data
				UpdateSensorDelayAdapters();
				UpdateAnalyses();

				if ( GetStoreData() )
					StoreCurrentFrame();

				// terminate when simulation has ended
				if ( HasSimulationEnded() )
				{
					log::DebugF( "Terminating simulation at %.3f", m_pTkTimeStepper->getTime() );
					break;
				}
			}
		}
		else
		{
			// Integrate from initial time to final time (the old way)
			m_pOsimManager->setFinalTime( time );
			m_pOsimManager->integrate( GetTkState() );
		}
	}

	void ModelOpenSim3::RequestTermination()
	{
		Model::RequestTermination();
		m_pOsimManager->halt(); // needed when using an OpenSim::Manager
	}

	double ModelOpenSim3::GetTime() const
	{
		return GetTkState().getTime();
	}

	int ModelOpenSim3::GetIntegrationStep() const
	{
		return GetTkIntegrator().getNumStepsTaken();
	}

	int ModelOpenSim3::GetPreviousIntegrationStep() const
	{
		return m_PrevIntStep;
	}

	double ModelOpenSim3::GetPreviousTime() const
	{
		return m_PrevTime;
	}

	Real ModelOpenSim3::GetTotalEnergyConsumption() const
	{
		if ( m_pProbe )
			return m_pProbe->getProbeOutputs( GetTkState() )[ 0 ];
		else return 0.0;
	}

	double ModelOpenSim3::GetSimulationEndTime() const
	{
		return m_pOsimManager->getFinalTime();
	}

	void ModelOpenSim3::SetSimulationEndTime( double t )
	{
		m_pOsimManager->setFinalTime( t );
		m_pTkIntegrator->setFinalTime( t );
	}

	const String& ModelOpenSim3::GetName() const
	{
		return GetOsimModel().getName();
	}

	void ModelOpenSim3::ReadState( const path& file )
	{
		// create a copy of the storage
		auto store = g_StorageCache( file.str() );
		OpenSim::Array< double > data = store->getStateVector( 0 )->getData();
		OpenSim::Array< std::string > storeLabels = store->getColumnLabels();

		// for all storage channels, check if there's a matching state
		for ( int i = 0; i < storeLabels.getSize(); i++ )
		{
			index_t idx = m_State.GetIndex( storeLabels[ i ] );
			if ( idx != NoIndex )
				m_State[ idx ] = data[ store->getStateIndex( storeLabels[ i ] ) ];
		}
	}

	void ModelOpenSim3::FixTkState( double force_threshold /*= 0.1*/, double fix_accuracy /*= 0.1 */ )
	{
		const Real step_size = 0.1;
		const Real max_range = 10.0; // don't look further than 10 meters up or down

		if ( GetState().GetIndex( initial_load_dof ) == NoIndex )
		{
			log::warning( "Ignoring initial load setting, could not find ", initial_load_dof );
			return;
		}

		// find top
		double initial_state = GetOsimModel().getStateVariable( GetTkState(), initial_load_dof );
		double top = initial_state;
		while ( abs( GetTotalContactForce() ) > force_threshold && ( top - initial_state < max_range ) )
		{
			top += step_size;
			GetOsimModel().setStateVariable( GetTkState(), initial_load_dof, top );
		}

		// find bottom
		double bottom = top;
		do
		{
			bottom -= step_size;
			GetOsimModel().setStateVariable( GetTkState(), initial_load_dof, bottom );
		}
		while ( abs( GetTotalContactForce() ) <= force_threshold && ( bottom - initial_state > -max_range ) );

		// find middle ground until we are close enough
		double force;
		double new_ty;
		for ( int i = 0; i < 100; ++i )
		{
			new_ty = ( top + bottom ) / 2;
			GetOsimModel().setStateVariable( GetTkState(), initial_load_dof, new_ty );
			force = abs( GetTotalContactForce() );

			// check if it's good enough
			if ( abs( force - force_threshold ) / force_threshold <= fix_accuracy )
				break;

			// update top / bottom
			if ( force > force_threshold ) bottom = new_ty; else top = new_ty;
		}

		if ( abs( force - force_threshold ) / force_threshold > fix_accuracy )
		{
			log::warning( "Could not find initial state for ", initial_load_dof, " with external force of ", force_threshold );
			GetOsimModel().setStateVariable( GetTkState(), initial_load_dof, initial_state );
		}
		//else
		//	log::TraceF( "Fixed initial state, new_ty=%.6f top=%.6f bottom=%.6f force=%.6f (target=%.6f)", new_ty, top, bottom, force, force_threshold );
	}

	void ModelOpenSim3::InitStateFromTk()
	{
		SCONE_ASSERT( GetState().GetSize() == 0 );
		auto osnames = GetOsimModel().getStateVariableNames();
		auto osvalues = GetOsimModel().getStateValues( GetTkState() );
		for ( int i = 0; i < osnames.size(); ++i )
			m_State.AddVariable( osnames[ i ], osvalues[ i ] );
	}

	void ModelOpenSim3::CopyStateFromTk()
	{
		SCONE_ASSERT( m_State.GetSize() >= GetOsimModel().getNumStateVariables() );
		auto osvalues = GetOsimModel().getStateValues( GetTkState() );
		for ( int i = 0; i < osvalues.size(); ++i )
			m_State.SetValue( i, osvalues[ i ] );
	}

	void ModelOpenSim3::CopyStateToTk()
	{
		SCONE_ASSERT( m_State.GetSize() >= GetOsimModel().getNumStateVariables() );
		GetOsimModel().setStateValues( GetTkState(), &m_State.GetValues()[ 0 ] );

		// set locked coordinates
		auto& cs = GetOsimModel().updCoordinateSet();
		for ( int i = 0; i < cs.getSize(); ++i )
		{
			if ( cs.get( i ).getLocked( GetTkState() ) )
			{
				cs.get( i ).setLocked( GetTkState(), false );
				cs.get( i ).setLocked( GetTkState(), true );
			}
		}
	}

	void ModelOpenSim3::SetState( const State& state, TimeInSeconds timestamp )
	{
		m_State.SetValues( state.GetValues() );
		CopyStateToTk();
		GetTkState().setTime( timestamp );
		m_pOsimModel->getMultibodySystem().realize( GetTkState(), SimTK::Stage::Acceleration );
		if ( GetController() )
			UpdateControlValues();
	}

	void ModelOpenSim3::SetStateValues( const std::vector< Real >& state, TimeInSeconds timestamp )
	{
		m_State.SetValues( state );
		CopyStateToTk();
		GetTkState().setTime( timestamp );
		m_pOsimModel->getMultibodySystem().realize( GetTkState(), SimTK::Stage::Acceleration );
		if ( GetController() )
			UpdateControlValues();
		if ( GetStoreData() )
			StoreCurrentFrame();
	}

	TimeInSeconds ModelOpenSim3::GetSimulationStepSize()
	{
		SCONE_ASSERT( use_fixed_control_step_size );
		return fixed_control_step_size;
	}

	void ModelOpenSim3::ValidateDofAxes()
	{
		SimTK::Matrix jsmat;
		m_pOsimModel->getMatterSubsystem().calcSystemJacobian( GetTkState(), jsmat );

		// extract axes from system Jacobian
		for ( auto coIdx = 0u; coIdx < m_Dofs.size(); ++coIdx )
		{
			DofOpenSim3& dof = static_cast<DofOpenSim3&>( *m_Dofs[ coIdx ] );
			auto mbIdx = dof.GetOsCoordinate().getJoint().getBody().getIndex();

			for ( auto j = 0; j < 3; ++j )
				dof.m_RotationAxis[ j ] = jsmat( mbIdx * 6 + j, coIdx );
		}
	}

	void ModelOpenSim3::UpdateOsimStorage()
	{
		OpenSim::Array<double> stateValues;
		m_pOsimModel->getStateValues( GetTkState(), stateValues );
		OpenSim::StateVector vec;
		vec.setStates( GetTkState().getTime(), stateValues.getSize(), &stateValues[ 0 ] );
		m_pOsimManager->getStateStorage().append( vec );
	}

	void ModelOpenSim3::InitializeOpenSimMuscleActivations( double override_activation )
	{
		for ( auto iter = GetMuscles().begin(); iter != GetMuscles().end(); ++iter )
		{
			OpenSim::Muscle& osmus = dynamic_cast<MuscleOpenSim3*>( iter->get() )->GetOsMuscle();
			auto a = override_activation != 0.0 ? override_activation : ( *iter )->GetInput();
			osmus.setActivation( GetOsimModel().updWorkingState(), a );
		}

		m_pOsimModel->equilibrateMuscles( GetTkState() );
	}

	void ModelOpenSim3::SetController( ControllerUP c )
	{
		SCONE_PROFILE_FUNCTION;

		Model::SetController( std::move( c ) );

		// Initialize muscle dynamics STEP 1
		// equilibrate with initial small actuation so we can update the sensor delay adapters (needed for reflex controllers)
		InitializeOpenSimMuscleActivations( initial_equilibration_activation );
		UpdateSensorDelayAdapters();

		// Initialize muscle dynamics STEP 2
		// compute actual initial control values and re-equilibrate muscles
		UpdateControlValues();
		InitializeOpenSimMuscleActivations();
	}
}
