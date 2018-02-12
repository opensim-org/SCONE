#include "scone/core/Exception.h"
#include "scone/core/Log.h"
#include "scone/core/propnode_tools.h"
#include "scone/core/Factories.h"
#include "scone/core/StorageIo.h"

#include "Model_Simbody.h"
#include "Body_Simbody.h"
#include "Muscle_Simbody.h"
#include "Simulation_Simbody.h"
#include "Joint_Simbody.h"
#include "Dof_Simbody.h"
#include "simbody_tools.h"

#include <OpenSim/OpenSim.h>
#include <OpenSim/Simulation/Model/Umberger2010MuscleMetabolicsProbe.h>
#include <OpenSim/Simulation/Model/Bhargava2004MuscleMetabolicsProbe.h>

#include "scone/core/system_tools.h"
#include "scone/core/Profiler.h"

#include "xo/string/string_tools.h"
#include "xo/string/pattern_matcher.h"
#include "xo/container/container_tools.h"
#include "xo/utility/file_resource_cache.h"

#include <thread>
#include <mutex>

using std::cout;
using std::endl;

namespace scone
{
	std::mutex g_SimBodyMutex;

	xo::file_resource_cache< OpenSim::Model > g_ModelCache( [&]( const path& p ) { return new OpenSim::Model( p.string() ); } );
	xo::file_resource_cache< OpenSim::Storage > g_StorageCache( [&]( const path& p ) { return new OpenSim::Storage( p.string() ); } );

	/// Simbody controller that calls scone controllers
	class ControllerDispatcher : public OpenSim::Controller
	{
	public:
		ControllerDispatcher( Model_Simbody& model ) : m_Model( model ) { };
		virtual void computeControls( const SimTK::State& s, SimTK::Vector &controls ) const override;
		virtual ControllerDispatcher* clone() const override { return new ControllerDispatcher( *this ); }
		virtual const std::string& getConcreteClassName() const override { SCONE_THROW_NOT_IMPLEMENTED; }

	private:
		Model_Simbody& m_Model;
	};

	/// Constructor
	Model_Simbody::Model_Simbody( const PropNode& props, Params& par ) :
		Model( props, par ),
		m_pOsimModel( nullptr ),
		m_pTkState( nullptr ),
		m_pControllerDispatcher( nullptr ),
		m_PrevIntStep( -1 ),
		m_PrevTime( 0.0 ),
		m_pProbe( 0 ),
		m_Mass( 0.0 ),
		m_BW( 0.0 )
	{
		SCONE_PROFILE_FUNCTION;

		path model_file;
		path state_init_file;
		String probe_class;
		double pre_control_simulation_time;
		double initial_leg_load;

		INIT_PROPERTY( props, integration_accuracy, 0.001 );
		INIT_PROPERTY( props, integration_method, String( "RungeKuttaMerson" ) );
		INIT_PROPERTY( props, max_step_size, 0.001 );
		INIT_PROPERTY( props, fixed_control_step_size, 0.001 );
		INIT_PROPERTY( props, use_fixed_control_step_size, fixed_control_step_size > 0 );

		INIT_PROPERTY_REQUIRED( props, model_file );
		INIT_PROPERTY( props, state_init_file, path() );
		INIT_PROPERTY( props, probe_class, String() );
		INIT_PROPERTY( props, pre_control_simulation_time, 0.0 );
		INIT_PROPERTY( props, initial_leg_load, 0.2 );

		INIT_PROPERTY( props, create_body_forces, false );

		// always set create_body_forces when there's a PerturbationController
		// TODO: think of a nicer, more generic way of dealing with this issue
		if ( auto* controllers = props.try_get_child( "Controllers" ) )
		{
			for ( auto& cprops : *controllers )
				create_body_forces |= cprops.second.get<string>( "type" ) == "PerturbationController";
		}

		// create new OpenSim Model using resource cache
		{
			SCONE_PROFILE_SCOPE( "CreateModel" );
			model_file = FindFile( model_file );
			m_pOsimModel = g_ModelCache( model_file );
			AddExternalResource( model_file );
		}

		// create torque and point actuators
		if ( create_body_forces )
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
			SCONE_PROFILE_SCOPE( "SetupOpenSimParameters" );

			// change model properties
			SetOpenSimParameters( props, par );

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
					//double mass = mus.getOptimalFiberLength() * mus.getMaxIsometricForce() / 23500.0; // Wronlgy derived from [Wang2012], should be 235?
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
			g_SimBodyMutex.lock();
			m_pTkState = &m_pOsimModel->initSystem();
			g_SimBodyMutex.unlock();
		}

		// create model component wrappers and sensors
		{
			SCONE_PROFILE_SCOPE( "CreateWrappers" );
			CreateModelWrappers( props, par );
			SetModelProperties( props, par );
			CreateBalanceSensors( props, par );
		}

		{
			SCONE_PROFILE_SCOPE( "InitVariables" );
			// initialize cached variables to save computation time
			m_Mass = m_pOsimModel->getMultibodySystem().getMatterSubsystem().calcSystemMass( m_pOsimModel->getWorkingState() );
			m_BW = GetGravity().length() * GetMass();
			ValidateDofAxes();
		}

		// Create the integrator for the simulation.
		{
			SCONE_PROFILE_SCOPE( "InitIntegrators" );

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
			if ( auto iso = props.try_get_child( "state_init_optimization" ) )
			{
				bool symmetric = iso->get< bool >( "symmetric", false );
				auto inc_pat = xo::pattern_matcher( iso->get< String >( "include_states", "*" ), ";" );
				auto ex_pat = xo::pattern_matcher( iso->get< String >( "exclude_states", "" ) + ";*.activation;*.fiber_length", ";" );
				for ( Index i = 0; i < m_State.GetSize(); ++i )
				{
					const String& state_name = m_State.GetName( i );
					if ( inc_pat( state_name ) && !ex_pat( state_name ) )
					{
						auto par_name = symmetric ? GetNameNoSide( state_name ) : state_name;
						if ( iso->has_key( "offset" ) )
							m_State[ i ] += par.get( par_name + ".offset", iso->get_child( "offset" ) );
						else m_State[ i ] += par.get( par_name + ".offset", iso->get< Real >( "init_mean", 0.0 ), iso->get< Real >( "init_std" ), iso->get< Real >( "min", -1000 ), iso->get< Real >( "max", 1000 ) );
					}
				}
			}

			// apply and fix state
			CopyStateToTk();
			FixTkState( initial_leg_load * GetBW() );
			CopyStateFromTk();
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

		{
			SCONE_PROFILE_SCOPE( "InitMuscleDynamics" );
			// Initialize muscle dynamics STEP 1
			// equilibrate with initial small actuation so we can update the sensor delay adapters (needed for reflex controllers)
			InitializeOpenSimMuscleActivations( 0.05 );
			UpdateSensorDelayAdapters();

			// Initialize muscle dynamics STEP 2
			// compute actual initial control values and re-equilibrate muscles
			UpdateControlValues();
			InitializeOpenSimMuscleActivations();
		}

		log::debug( "Successfully constructed ", GetName(), "; dofs=", GetDofs().size(), " muscles=", GetMuscles().size(), " mass=", GetMass() );
	}

	Model_Simbody::~Model_Simbody() {}

	void Model_Simbody::CreateModelWrappers( const PropNode& pn, Params& par )
	{
		SCONE_ASSERT( m_pOsimModel );
		SCONE_ASSERT( m_Bodies.empty() && m_Joints.empty() && m_Dofs.empty() && m_Actuators.empty() );

		// Create wrappers for actuators
		m_Muscles.clear();
		for ( int idx = 0; idx < m_pOsimModel->getActuators().getSize(); ++idx )
		{
			// OpenSim: Set<T>::get( idx ) is const but returns non-const reference, is this a bug?
			OpenSim::Actuator& osAct = m_pOsimModel->getActuators().get( idx );
			if ( OpenSim::Muscle* osMus = dynamic_cast<OpenSim::Muscle*>( &osAct ) )
			{
				m_Muscles.push_back( MuscleUP( new Muscle_Simbody( *this, *osMus ) ) );
				m_Actuators.push_back( m_Muscles.back().get() );
			}
			else if ( OpenSim::PointActuator* osPa = dynamic_cast<OpenSim::PointActuator*>( &osAct ) )
			{
				// do something?
			}
		}

		// Create wrappers for bodies
		m_Bodies.clear();
		for ( int idx = 0; idx < m_pOsimModel->getBodySet().getSize(); ++idx )
			m_Bodies.push_back( BodyUP( new Body_Simbody( *this, m_pOsimModel->getBodySet().get( idx ) ) ) );

		// Create wrappers for joints
		//m_Joints.clear();
		//for ( int idx = 0; idx < m_pOsimModel->getJointSet().getSize(); ++idx )
		//	m_Joints.push_back( JointUP( new Joint_Simbody( *this, m_pOsimModel->getJointSet().get( idx ) ) ) );

		// create BodySensor
		//m_BalanceSensor = BalanceSensorUP( new BalanceSensor( * this ) );

		// setup hierarchy and create wrappers
		m_RootLink = CreateLinkHierarchy( m_pOsimModel->getGroundBody() );

		// create wrappers for dofs
		m_Dofs.clear();
		for ( int idx = 0; idx < m_pOsimModel->getCoordinateSet().getSize(); ++idx )
		{
			m_Dofs.push_back( DofUP( new Dof_Simbody( *this, m_pOsimModel->getCoordinateSet().get( idx ) ) ) );
			//m_ChannelSensors.push_back( m_Dofs.back().get() );
		}

		// create legs and connect stance_contact forces
		if ( Link* left_femur = m_RootLink->FindLink( "femur_l" ) )
		{
			Link& left_foot = left_femur->GetChild( 0 ).GetChild( 0 );
			m_Legs.push_back( LegUP( new Leg( *left_femur, left_foot, m_Legs.size(), LeftSide ) ) );
			dynamic_cast<Body_Simbody&>( left_foot.GetBody() ).ConnectContactForce( "foot_l" );
		}

		if ( Link* right_femur = m_RootLink->FindLink( "femur_r" ) )
		{
			Link& right_foot = right_femur->GetChild( 0 ).GetChild( 0 );
			m_Legs.push_back( LegUP( new Leg( *right_femur, right_femur->GetChild( 0 ).GetChild( 0 ), m_Legs.size(), RightSide ) ) );
			dynamic_cast<Body_Simbody&>( right_foot.GetBody() ).ConnectContactForce( "foot_r" );
		}
	}

	void Model_Simbody::SetModelProperties( const PropNode &pn, Params& par )
	{
		if ( auto* model_props = pn.try_get_child( "ModelProperties" ) )
		{
			for ( auto& mp : *model_props )
			{
				int usage = 0;
				if ( mp.first == "Actuator" )
				{
					for ( auto act : xo::make_view_if( m_Actuators, xo::pattern_matcher( mp.second.get< String >( "name" ) ) ) )
					{
						SCONE_THROW_IF( !use_fixed_control_step_size, "Custom Actuator Delay only works with use_fixed_control_step_size" );
						act->SetActuatorDelay( mp.second.get< TimeInSeconds >( "delay", 0.0 ) * sensor_delay_scaling_factor, fixed_control_step_size );
						++usage;
					}
				}

				if ( usage == 0 )
					log::warning( "Unused model property: ", mp.second.get< String >( "name" ) );
			}
		}
	}


	void Model_Simbody::SetOpenSimParameters( const PropNode& props, Params& par )
	{
		if ( auto* osim_pars = props.try_get_child( "OpenSimParameters" ) )
		{
			for ( auto param_it = osim_pars->begin(); param_it != osim_pars->end(); ++param_it )
			{
				xo::pattern_matcher pm( param_it->second.get< String >( "name" ) );
				if ( param_it->first == "Force" )
				{
					for ( int i = 0; i < m_pOsimModel->updMuscles().getSize(); ++i )
					{
						auto& osMus = m_pOsimModel->updMuscles().get( i );
						if ( pm( osMus.getName() ) )
							SetOpenSimParameter( osMus, param_it->second, par );
					}
				}
			}
		}
	}

	void Model_Simbody::SetOpenSimParameter( OpenSim::Object& os, const PropNode& pn, Params& par )
	{
		// we have a match!
		String prop_str = pn.get< String >( "property" );
		ScopedParamSetPrefixer prefix( par, pn.get< String >( "name" ) + "." );
		double value = par.get( prop_str, pn.get_child( "value" ) );
		if ( os.hasProperty( prop_str ) )
		{
			auto& prop = os.updPropertyByName( prop_str ).updValue< double >();
			prop = pn.get( "scale", false ) ? prop * value : value;
		}
	}

	String Model_Simbody::WriteResult( const path& file ) const
	{
		//WriteStorageSto( m_Data, ( file + ".sto" ).str(), ( file.parent_path().filename() / file.stem() ).str() );
		// write results
		for ( auto& c : GetControllers() )
			c->WriteResult( file );

		return file.str();
	}

	Vec3 Model_Simbody::GetComPos() const
	{
		return ToVec3( m_pOsimModel->calcMassCenterPosition( GetTkState() ) );
	}

	Vec3 Model_Simbody::GetComVel() const
	{
		return ToVec3( m_pOsimModel->calcMassCenterVelocity( GetTkState() ) );
	}

	Vec3 Model_Simbody::GetComAcc() const
	{
		return ToVec3( m_pOsimModel->calcMassCenterAcceleration( GetTkState() ) );
	}

	scone::Vec3 Model_Simbody::GetGravity() const
	{
		return ToVec3( m_pOsimModel->getGravity() );
	}

	bool is_body_equal( BodyUP& body, OpenSim::Body& osBody )
	{
		return dynamic_cast<Body_Simbody&>( *body ).m_osBody == osBody;
	}

	scone::LinkUP Model_Simbody::CreateLinkHierarchy( OpenSim::Body& osBody, Link* parent )
	{
		LinkUP link;

		// find the Body
		auto itBody = std::find_if( m_Bodies.begin(), m_Bodies.end(), [&]( BodyUP& body ) { return dynamic_cast<Body_Simbody&>( *body ).m_osBody == osBody; } );
		SCONE_ASSERT( itBody != m_Bodies.end() );

		// find the Joint (if any)
		if ( osBody.hasJoint() )
		{
			// create a joint
			m_Joints.push_back( JointUP( new Joint_Simbody( **itBody, parent ? &parent->GetJoint() : nullptr, *this, osBody.getJoint() ) ) );
			//auto itJoint = std::find_if( m_Joints.begin(), m_Joints.end(), [&]( JointUP& body ){ return dynamic_cast< Joint_Simbody& >( *body ).m_osJoint == osBody.getJoint(); } );
			//SCONE_ASSERT( itJoint != m_Joints.end() );
			link = LinkUP( new Link( **itBody, *m_Joints.back(), parent ) );
		}
		else
		{
			// this is the root Link
			link = LinkUP( new Link( **itBody ) );
		}

		// add children
		for ( auto iter = m_Bodies.begin(); iter != m_Bodies.end(); ++iter )
		{
			Body_Simbody& childBody = dynamic_cast<Body_Simbody&>( **iter );
			if ( childBody.m_osBody.hasJoint() && childBody.m_osBody.getJoint().getParentBody() == osBody )
			{
				// create child link
				link->GetChildren().push_back( CreateLinkHierarchy( childBody.m_osBody, link.get() ) );
			}
		}

		return link;
	}

	void Model_Simbody::ClearBodyForces()
	{
		for ( auto& bf : m_BodyForces )
			bf->setNull();
	}

	void ControllerDispatcher::computeControls( const SimTK::State& s, SimTK::Vector &controls ) const
	{
		SCONE_PROFILE_FUNCTION;

		// see 'catch' statement below for explanation try {} catch {} is needed
		try
		{
			if ( !m_Model.use_fixed_control_step_size )
			{
				// update current state (TODO: remove const cast)
				m_Model.SetTkState( const_cast<SimTK::State&>( s ) );

				// update SensorDelayAdapters at the beginning of each new step
				// TODO: move this to an analyzer object or some other point
				if ( m_Model.GetIntegrationStep() > m_Model.m_PrevIntStep && m_Model.GetIntegrationStep() > 0 )
				{
					m_Model.UpdateSensorDelayAdapters();
					m_Model.UpdateAnalyses();
				}

				// update actuator values
				m_Model.ClearBodyForces();
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
				SCONE_ASSERT_MSG( controls.size() == m_Model.GetMuscles().size(), "Only muscle actuators are supported in SCONE at this moment" );

				int idx = 0;
				for ( MuscleUP& mus : m_Model.GetMuscles() )
				{
					// This is an optimization that only works when there are only muscles
					// OpenSim: addInControls is rather inefficient, that's why we don't use it
					// TODO: fix this into a generic version (i.e. work with other actuators)
					controls[ idx++ ] += mus->GetInput();
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


	void Model_Simbody::StoreCurrentFrame()
	{
		SCONE_PROFILE_FUNCTION;

		// store scone data
		Model::StoreCurrentFrame();
	}

	bool Model_Simbody::AdvanceSimulationTo( double final_time )
	{
		SCONE_PROFILE_FUNCTION;
		SCONE_ASSERT( m_pOsimManager );

		try
		{
			std::unique_lock< std::mutex > lock( GetSimulationMutex(), std::defer_lock );
			if ( GetThreadSafeSimulation() )
				lock.lock();

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
				int number_of_steps = static_cast<int>( 0.5 + ( final_time - GetTime() ) / fixed_control_step_size );
				int thread_interuption_steps = static_cast<int>( std::max( 10.0, 0.02 / fixed_control_step_size ) );

				for ( int current_step = 0; current_step < number_of_steps; )
				{
					// update controls
					ClearBodyForces();
					UpdateControlValues();

					// integrate
					m_PrevTime = GetTime();
					m_PrevIntStep = GetIntegrationStep();
					double target_time = GetTime() + fixed_control_step_size;
					SimTK::Integrator::SuccessfulStepStatus status;

					{
						SCONE_PROFILE_SCOPE( "SimTK::TimeStepper::stepTo" );
						status = m_pTkTimeStepper->stepTo( target_time );
					}

					SetTkState( m_pTkIntegrator->updAdvancedState() );
					CopyStateFromTk();

					++current_step;

					// Realize Acceleration, analysis components may need it
					// this way the results are always consistent
					m_pOsimModel->getMultibodySystem().realize( GetTkState(), SimTK::Stage::Acceleration );

					// update the sensor delays, analyses, and store data
					UpdateSensorDelayAdapters();
					UpdateAnalyses();

					if ( GetStoreData() )
						StoreCurrentFrame();

					// terminate on request
					if ( GetTerminationRequest() || status == SimTK::Integrator::EndOfSimulation )
					{
						log::DebugF( "Terminating simulation at %.3f", m_pTkTimeStepper->getTime() );
						break;
					}

					// allow time for other threads to access the model
					if ( GetThreadSafeSimulation() && current_step % thread_interuption_steps == 0 )
					{
						// notify GUI thread
						lock.unlock();
						GetSimulationCondVar().notify_all();
						lock.lock();
					}
				}
			}
			else
			{
				// Integrate from initial time to final time (the old way)
				m_pOsimManager->setFinalTime( final_time );
				m_pOsimManager->integrate( GetTkState() );
			}
		}
		catch ( std::exception& e )
		{
			// in case of an OpenSim exception, just log the exception and return false
			// TODO: use Result class
			log::error( e.what() );
			return false;
		}

		if ( GetThreadSafeSimulation() )
			GetSimulationCondVar().notify_all();

		return true;
	}

	void Model_Simbody::SetTerminationRequest()
	{
		Model::SetTerminationRequest();
		m_pOsimManager->halt();
	}

	double Model_Simbody::GetTime() const
	{
		return GetTkState().getTime();
	}

	int Model_Simbody::GetIntegrationStep() const
	{
		return GetTkIntegrator().getNumStepsTaken();
	}

	int Model_Simbody::GetPreviousIntegrationStep() const
	{
		return m_PrevIntStep;
	}

	double Model_Simbody::GetPreviousTime() const
	{
		return m_PrevTime;
	}

	std::ostream& Model_Simbody::ToStream( std::ostream& str ) const
	{
		Model::ToStream( str );

		GetOsimModel().getMultibodySystem().realize( *m_pTkState, SimTK::Stage::Dynamics );

		str << endl << "Forces:" << endl;
		const OpenSim::ForceSet& fset = GetOsimModel().getForceSet();
		for ( int i = 0; i < fset.getSize(); ++i )
		{
			OpenSim::Force& f = fset.get( i );
			str << f.getName() << endl;
			for ( int rec = 0; rec < f.getRecordLabels().size(); ++rec )
				str << "  " << f.getRecordLabels().get( rec ) << ": " << f.getRecordValues( *m_pTkState ).get( rec ) << endl;
		}

		return str;
	}

	scone::Real Model_Simbody::GetTotalEnergyConsumption() const
	{
		if ( m_pProbe )
			return m_pProbe->getProbeOutputs( GetTkState() )[ 0 ];
		else return 0.0;
	}

	double Model_Simbody::GetSimulationEndTime() const
	{
		return m_pOsimManager->getFinalTime();
	}

	void Model_Simbody::SetSimulationEndTime( double t )
	{
		m_pOsimManager->setFinalTime( t );
		m_pTkIntegrator->setFinalTime( t );
	}

	const String& Model_Simbody::GetName() const
	{
		return GetOsimModel().getName();
	}

	void Model_Simbody::ReadState( const path& file )
	{
		// create a copy of the storage
		auto store = g_StorageCache( file );
		OpenSim::Array< double > data = store->getStateVector( 0 )->getData();
		OpenSim::Array< std::string > storeLabels = store->getColumnLabels();

		// for all storage channels, check if there's a matching state
		for ( int i = 0; i < storeLabels.getSize(); i++ )
		{
			Index idx = m_State.GetIndex( storeLabels[ i ] );
			if ( idx != NoIndex )
				m_State[ idx ] = data[ store->getStateIndex( storeLabels[ i ] ) ];
		}
	}

	void Model_Simbody::FixTkState( double force_threshold /*= 0.1*/, double fix_accuracy /*= 0.1 */ )
	{
		const String state_name = "pelvis_ty";
		const Real step_size = 0.1;

		// find top
		double top = GetOsimModel().getStateVariable( GetTkState(), state_name );
		while ( abs( GetTotalContactForce() ) > force_threshold )
		{
			top += step_size;
			GetOsimModel().setStateVariable( GetTkState(), state_name, top );
		}

		// find bottom
		double bottom = top;
		do
		{
			bottom -= step_size;
			GetOsimModel().setStateVariable( GetTkState(), state_name, bottom );
		}
		while ( abs( GetTotalContactForce() ) <= force_threshold );

		// find middle ground until we are close enough
		double force;
		double new_ty;
		for ( int i = 0; i < 100; ++i )
		{
			new_ty = ( top + bottom ) / 2;
			GetOsimModel().setStateVariable( GetTkState(), state_name, new_ty );
			force = abs( GetTotalContactForce() );

			// check if it's good enough
			if ( abs( force - force_threshold ) / force_threshold <= fix_accuracy )
				break;

			// update top / bottom
			if ( force > force_threshold ) bottom = new_ty; else top = new_ty;
		}

		if ( abs( force - force_threshold ) / force_threshold > fix_accuracy )
			log::WarningF( "Could not fix initial state, new_ty=%.6f top=%.6f bottom=%.6f force=%.6f (target=%.6f)", new_ty, top, bottom, force, force_threshold );
		else
			log::TraceF( "Fixed initial state, new_ty=%.6f top=%.6f bottom=%.6f force=%.6f (target=%.6f)", new_ty, top, bottom, force, force_threshold );
	}

	//void Model_Simbody::SetTkState( const State& s )
	//{
	//	for ( Index i = 0; i < s.GetSize(); ++i )
	//		GetOsimModel().setStateVariable( GetTkState(), s.GetName( i ), s.GetValue( i ) );
	//}

	void Model_Simbody::InitStateFromTk()
	{
		SCONE_ASSERT( GetState().GetSize() == 0 );
		auto osnames = GetOsimModel().getStateVariableNames();
		auto osvalues = GetOsimModel().getStateValues( GetTkState() );
		for ( int i = 0; i < osnames.size(); ++i )
			GetState().AddVariable( osnames[ i ], osvalues[ i ] );
	}

	void Model_Simbody::CopyStateFromTk()
	{
		SCONE_ASSERT( m_State.GetSize() >= GetOsimModel().getNumStateVariables() );
		auto osvalues = GetOsimModel().getStateValues( GetTkState() );
		for ( int i = 0; i < osvalues.size(); ++i )
			m_State.SetValue( i, osvalues[ i ] );
	}

	void Model_Simbody::CopyStateToTk()
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

	void Model_Simbody::SetState( const State& state, TimeInSeconds timestamp )
	{
		m_State.SetValues( state.GetValues() );
		CopyStateToTk();
		GetTkState().setTime( timestamp );
		m_pOsimModel->getMultibodySystem().realize( GetTkState(), SimTK::Stage::Acceleration );
		UpdateControlValues();
	}

	void Model_Simbody::SetStateValues( const std::vector< Real >& state, TimeInSeconds timestamp )
	{
		m_State.SetValues( state );
		CopyStateToTk();
		GetTkState().setTime( timestamp );
		m_pOsimModel->getMultibodySystem().realize( GetTkState(), SimTK::Stage::Acceleration );
		UpdateControlValues();

		if ( GetStoreData() )
			StoreCurrentFrame();
	}

	TimeInSeconds Model_Simbody::GetSimulationStepSize()
	{
		SCONE_ASSERT( use_fixed_control_step_size );
		return fixed_control_step_size;
	}

	void Model_Simbody::ValidateDofAxes()
	{
		SimTK::Matrix jsmat;
		m_pOsimModel->getMatterSubsystem().calcSystemJacobian( GetTkState(), jsmat );

		// extract axes from system Jacobian
		for ( auto coIdx = 0u; coIdx < m_Dofs.size(); ++coIdx )
		{
			Dof_Simbody& dof = static_cast<Dof_Simbody&>( *m_Dofs[ coIdx ] );
			auto mbIdx = dof.GetOsCoordinate().getJoint().getBody().getIndex();

			for ( auto j = 0; j < 3; ++j )
				dof.m_RotationAxis[ j ] = jsmat( mbIdx * 6 + j, coIdx );
		}
	}

	void Model_Simbody::UpdateOsimStorage()
	{
		OpenSim::Array<double> stateValues;
		m_pOsimModel->getStateValues( GetTkState(), stateValues );
		OpenSim::StateVector vec;
		vec.setStates( GetTkState().getTime(), stateValues.getSize(), &stateValues[ 0 ] );
		m_pOsimManager->getStateStorage().append( vec );
	}

	void Model_Simbody::InitializeOpenSimMuscleActivations( double override_activation )
	{
		for ( auto iter = GetMuscles().begin(); iter != GetMuscles().end(); ++iter )
		{
			OpenSim::Muscle& osmus = dynamic_cast<Muscle_Simbody*>( iter->get() )->GetOsMuscle();
			auto a = override_activation != 0.0 ? override_activation : ( *iter )->GetInput();
			osmus.setActivation( GetOsimModel().updWorkingState(), a );
		}

		m_pOsimModel->equilibrateMuscles( GetTkState() );
	}
}
