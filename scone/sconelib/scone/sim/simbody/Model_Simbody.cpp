#include "scone/core/Exception.h"
#include "scone/core/Log.h"
#include "scone/core/propnode_tools.h"

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

#include "scone/sim/Factories.h"

#include <boost/thread.hpp>
#include <boost/filesystem.hpp>
#include "scone/core/system_tools.h"
#include "scone/core/Profiler.h"

#include <flut/string_tools.hpp>

#include "scone/core/StorageIo.h"

using std::cout;
using std::endl;

namespace scone
{
	namespace sim
	{
		boost::mutex g_SimBodyMutex;
		ResourceCache< OpenSim::Model > g_ModelCache;
		ResourceCache< OpenSim::Storage > g_StorageCache;

		/// Simbody controller that calls scone controllers
		class Model_Simbody::ControllerDispatcher : public OpenSim::Controller
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
		Model_Simbody::Model_Simbody( const PropNode& props, opt::ParamSet& par ) :
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
			SCONE_PROFILE_SCOPE;

			String model_file;
			String state_init_file;
			String probe_class;
			double pre_control_simulation_time;
			double initial_leg_load;

			INIT_PROPERTY( props, integration_accuracy, 0.0001 );
			INIT_PROPERTY( props, integration_method, String( "RungeKuttaMerson" ) );
			INIT_PROPERTY( props, max_step_size, 0.001 );
			INIT_PROPERTY( props, use_fixed_control_step_size, false );
			INIT_PROPERTY( props, fixed_control_step_size, 0.001 );

			INIT_PROPERTY_REQUIRED( props, model_file );
			INIT_PROPERTY( props, state_init_file, String() );
			INIT_PROPERTY( props, probe_class, String() );
			INIT_PROPERTY( props, pre_control_simulation_time, 0.0 );
			INIT_PROPERTY( props, initial_leg_load, 0.2 );

			// create new OpenSim Model using resource cache
			m_pOsimModel = g_ModelCache.CreateCopy( GetSconeFolder( "models" ) + model_file );

			// change model properties
			if ( props.HasKey( "SimbodyParameters" ) )
				SetOpenSimParameters( props.GetChild( "SimbodyParameters" ), par );

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

			// Initialize the system
			// This is not thread-safe in case an exception is thrown, so we add a mutex guard
			g_SimBodyMutex.lock();
			m_pTkState = &m_pOsimModel->initSystem();
			g_SimBodyMutex.unlock();

			// create model component wrappers and sensors
			CreateModelWrappers();
			CreateBalanceSensors( props, par );

			// initialize cached variables to save computation time
			m_Mass = m_pOsimModel->getMultibodySystem().getMatterSubsystem().calcSystemMass( m_pOsimModel->getWorkingState() );
			m_BW = GetGravity().length() * GetMass();

			ValidateDofAxes();

			// Create the integrator for the simulation.
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
			else SCONE_THROW( "Invalid integration method: " + quoted( integration_method ) );

			m_pTkIntegrator->setAccuracy( integration_accuracy );
			m_pTkIntegrator->setMaximumStepSize( max_step_size );
			m_pTkIntegrator->resetAllStatistics();

			// read initial state
			if ( !state_init_file.empty() )
			{
				std::map< String, Real > state = ReadState( GetSconeFolder( "models" ) + state_init_file );
				if ( auto& iso = props.TryGetChild( "state_init_optimization" ) )
				{
					for ( auto& nvp : state )
					{
						if ( flut::matches_pattern( nvp.first, iso.GetStr( "include_states" ) ) && !flut::matches_pattern( nvp.first, iso.GetStr( "exclude_states" ) ) )
							nvp.second += par.Get( opt::ParamInfo( nvp.first + ".offset", iso.GetReal( "init_mean", 0.0 ), iso.GetReal( "init_std" ), 0, 0, iso.GetReal( "min", -1000 ), iso.GetReal( "max", 1000 ) ) );
					}
				}
				SetStateVariables( state );
				FixState( initial_leg_load * GetBW() );
			}

			// Create a manager to run the simulation. Can change manager options to save run time and memory or print more information
			m_pOsimManager = std::unique_ptr< OpenSim::Manager >( new OpenSim::Manager( *m_pOsimModel, *m_pTkIntegrator ) );
			m_pOsimManager->setWriteToStorage( GetStoreData() );
			m_pOsimManager->setPerformAnalyses( false );
			m_pOsimManager->setInitialTime( 0.0 );

			// Realize acceleration because controllers may need it and in this way the results are consistent
			m_pOsimModel->getMultibodySystem().realize( GetTkState(), SimTK::Stage::Acceleration );

			// create and initialize controllers
			const PropNode& cprops = props.GetChild( "Controllers" ).Touch();
			for ( auto iter = cprops.Begin(); iter != cprops.End(); ++iter )
				m_Controllers.push_back( CreateController( *iter->second, par, *this, sim::Area::WHOLE_BODY ) );

			// Initialize muscle dynamics

			// STEP 1: equilibrate with initial small actuation so we can update the sensor delay adapters (needed for reflex controllers)
			InitializeOpenSimMuscleActivations( 0.05 );
			UpdateSensorDelayAdapters();

			// STEP 2: compute actual initial control values and re-equilibrate muscles
			UpdateControlValues();
			InitializeOpenSimMuscleActivations();

			log::debug( "Successfully constructed ", GetName(), "; dofs=", GetDofs().size(), " muscles=", GetMuscles().size(), " mass=", GetMass() );
		}

		Model_Simbody::~Model_Simbody() {}

		void Model_Simbody::CreateModelWrappers()
		{
			SCONE_ASSERT( m_pOsimModel );
			SCONE_ASSERT( m_Bodies.empty() && m_Joints.empty() && m_Dofs.empty() && m_Actuators.empty() );

			// Create wrappers for actuators
			m_Muscles.clear();
			for ( int idx = 0; idx < m_pOsimModel->getActuators().getSize(); ++idx )
			{
				// OpenSim: Set<T>::get( idx ) is const but returns non-const reference, is this a bug?
				OpenSim::Actuator& osAct = m_pOsimModel->getActuators().get( idx );

				try // see if it's a muscle
				{
					OpenSim::Muscle& osMus = dynamic_cast<OpenSim::Muscle&>( osAct );
					m_Muscles.push_back( MuscleUP( new Muscle_Simbody( *this, osMus ) ) );
					//m_ChannelSensors.push_back( m_Muscles.back().get() );
					m_Actuators.push_back( m_Muscles.back().get() );
				}
				catch ( std::bad_cast& )
				{
					SCONE_THROW( "Unsupported actuator type" );
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

		String Model_Simbody::WriteData( const String& file ) const
		{
			boost::filesystem::path path( file + ".sto" );
			auto name = ( path.parent_path().filename() / path.stem() ).string();

			// write scone data
			WriteStorageSto( m_Data, path.string(), name );

			return path.string();
		}

		Vec3 Model_Simbody::GetComPos() const
		{
			return ToVec3( m_pOsimModel->calcMassCenterPosition( GetTkState() ) );
		}

		Vec3 Model_Simbody::GetComVel() const
		{
			return ToVec3( m_pOsimModel->calcMassCenterVelocity( GetTkState() ) );
		}

		scone::Vec3 Model_Simbody::GetGravity() const
		{
			return ToVec3( m_pOsimModel->getGravity() );
		}

		bool is_body_equal( BodyUP& body, OpenSim::Body& osBody )
		{
			return dynamic_cast<Body_Simbody&>( *body ).m_osBody == osBody;
		}

		scone::sim::LinkUP Model_Simbody::CreateLinkHierarchy( OpenSim::Body& osBody, Link* parent )
		{
			LinkUP link;

			// find the sim::Body
			auto itBody = std::find_if( m_Bodies.begin(), m_Bodies.end(), [&]( BodyUP& body ){ return dynamic_cast<Body_Simbody&>( *body ).m_osBody == osBody; } );
			SCONE_ASSERT( itBody != m_Bodies.end() );

			// find the sim::Joint (if any)
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

		void Model_Simbody::ControllerDispatcher::computeControls( const SimTK::State& s, SimTK::Vector &controls ) const
		{
			SCONE_PROFILE_SCOPE;

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
					SCONE_PROFILE_SCOPE_NAMED( "addInControls" );
					SimTK::Vector controlValue( 1 );
					int idx = 0;
					for ( MuscleUP& mus: m_Model.GetMuscles() )
					{
						// This is an optimization that only works when there are only muscles
						// OpenSim: addInControls is rather inefficient, that's why we don't use it
						// TODO: fix this into a generic version (i.e. work with other actuators)
						controls[ idx++ ] += mus->GetControlValue();
					}
				}
			}
			catch ( std::exception& e )
			{
				// exceptions are caught and reported here
				// otherwise they get lost in SimTK::AbstractIntegratorRep::attemptDAEStep()
				// OpenSim: please remove the catch(...) statement
				log::Critical( e.what() );
				throw e;
			}
		}


		void Model_Simbody::StoreCurrentFrame()
		{
			SCONE_PROFILE_SCOPE;

			// store scone data
			Model::StoreCurrentFrame();
		}

		bool Model_Simbody::AdvanceSimulationTo( double final_time )
		{
			SCONE_PROFILE_SCOPE;
			SCONE_ASSERT( m_pOsimManager );

			try
			{
				if ( use_fixed_control_step_size )
				{
					// set this because it's used by GetSimulationEndTime()
					// TODO: Change this!
					m_pOsimManager->setFinalTime( final_time );

					// Integrate using time stepper
					m_pTkIntegrator->setFinalTime( final_time );
					SimTK::TimeStepper ts( m_pOsimModel->getMultibodySystem(), *m_pTkIntegrator );
					ts.initialize( GetTkState() );

					if ( GetStoreData() )
					{
						// store initial frame
						m_pOsimModel->getMultibodySystem().realize( GetTkState(), SimTK::Stage::Acceleration );
						StoreCurrentFrame();
					}

					// start integration loop
					int number_of_steps = static_cast<int>( 0.5 + final_time / fixed_control_step_size );
					for ( int current_step = 0; current_step < number_of_steps; )
					{
						// update controls
						UpdateControlValues();

						// integrate
						m_PrevTime = GetTime();
						m_PrevIntStep = GetIntegrationStep();
						double target_time = GetTime() + fixed_control_step_size;
						SimTK::Integrator::SuccessfulStepStatus status = ts.stepTo( target_time );
						SetTkState( m_pTkIntegrator->updAdvancedState() );
						//SetTkState( const_cast< SimTK::State& >( ts.getState() ) );

						++current_step;

						// Realize Acceleration, analysis components may need it
						// this way the results are always consistent
						m_pOsimModel->getMultibodySystem().realize( GetTkState(), SimTK::Stage::Acceleration );

						// update the sensor delays
						UpdateSensorDelayAdapters();

						// call UpdateAnalysis() on all controllers
						UpdateAnalyses();

						// store external data for later analysis
						if ( GetStoreData() )
							StoreCurrentFrame();

						// terminate on request
						if ( GetTerminationRequest() || status == SimTK::Integrator::EndOfSimulation )
						{
							log::DebugF( "Terminating simulation at %.6f", ts.getTime() );
							// TODO: return appropriate result
							break;
						}

						//log::TraceF( "cur=%03d Int=%03d Prev=%03d %.6f %.6f", current_step, GetIntegrationStep(), GetPreviousIntegrationStep(), current_time, GetTime() );
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
				log::Error( e.what() );
				return false;
			}
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

		std::map< String, Real > Model_Simbody::ReadState( const String& file )
		{
			// OpenSim: why is there no normal way to get a value using a label???

			// create a copy of the storage
			auto store = g_StorageCache.CreateCopy( file );
			OpenSim::Array< double > data = store->getStateVector( 0 )->getData();
			OpenSim::Array< std::string > storeLabels = store->getColumnLabels();
			OpenSim::Array< std::string > stateNames = GetOsimModel().getStateVariableNames();

			// run over all labels
			std::map< String, Real > state;
			for ( int i = 0; i < storeLabels.getSize(); i++ )
			{
				// check if the label is corresponds to a state
				if ( stateNames.findIndex( storeLabels[ i ] ) != -1 )
				{
					state[ storeLabels[ i ] ] = data[ store->getStateIndex( storeLabels[ i ] ) ];
				}
				//else log::Trace( "Unused state parameter: " + storeLabels[ i ] );
			}

			return state;
		}

		double Model_Simbody::GetSimulationEndTime() const
		{
			return m_pOsimManager->getFinalTime();
		}

		scone::String Model_Simbody::GetClassSignature() const
		{
			return GetOsimModel().getName();
		}

		const String& Model_Simbody::GetName() const
		{
			return GetOsimModel().getName();
		}

		void Model_Simbody::FixState( double force_threshold /*= 0.1*/, double fix_accuracy /*= 0.1 */ )
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
			} while ( abs( GetTotalContactForce() ) <= force_threshold );

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

		void Model_Simbody::SetStateVariables( const std::map< String, Real >& state )
		{
			for ( const auto& nvp : state )
				GetOsimModel().setStateVariable( GetTkState(), nvp.first, nvp.second );
		}

		void Model_Simbody::SetStoreData( bool store )
		{
			Model::SetStoreData( store );
			m_pOsimManager->setWriteToStorage( store );
		}

		std::vector< String > Model_Simbody::GetStateVariableNames() const
		{
			auto osnames = GetOsimModel().getStateVariableNames();
			std::vector< String > state_names( osnames.size() );

			for ( int i = 0; i < osnames.size(); ++i )
				state_names[ i ] = osnames[ i ];

			return state_names;
		}

		std::vector< Real > Model_Simbody::GetStateValues() const
		{
			auto osvalues = GetOsimModel().getStateValues( GetTkState() );
			std::vector< Real > state_values( osvalues.size() );

			for ( int i = 0; i < osvalues.size(); ++i )
				state_values[ i ] = osvalues[ i ];

			return state_values;
		}

		void Model_Simbody::SetStateValues( const std::vector< Real >& state_vars )
		{
			std::vector< double > state_vars_d = state_vars;
			GetOsimModel().setStateValues( GetTkState(), &state_vars_d[ 0 ] );
		}

		Real Model_Simbody::GetStateVariable( const String& name ) const
		{
			return GetOsimModel().getStateVariable( GetTkState(), name );
		}

		void Model_Simbody::SetStateVariable( const String& name, Real value )
		{
			GetOsimModel().setStateVariable( GetTkState(), name, value );
		}

		void Model_Simbody::SetOpenSimParameters( const PropNode& props, opt::ParamSet& par )
		{
			auto forceIt = props.FindChild( "ForceSet" );
			if ( forceIt != props.End() )
			{
				opt::ScopedParamSetPrefixer prefix1( par, "ForceSet." );
				for ( auto musIt = forceIt->second->Begin(); musIt != forceIt->second->End(); ++musIt )
				{
					opt::ScopedParamSetPrefixer prefix2( par, musIt->first + "." );
					auto& osForce = m_pOsimModel->updForceSet().get( musIt->first );
					for ( auto musPropIt = musIt->second->Begin(); musPropIt != musIt->second->End(); ++musPropIt )
					{
						double value = par.Get( musPropIt->first, *musIt->second, musPropIt->first );
						osForce.updPropertyByName( musPropIt->first ).updValue< double >() = value;
					}
				}
			}
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
			vec.setStates( GetTkState().getTime(), stateValues.getSize(), &stateValues[0] );
			m_pOsimManager->getStateStorage().append( vec );
		}

		void Model_Simbody::InitializeOpenSimMuscleActivations( double override_activation )
		{
			for ( auto iter = GetMuscles().begin(); iter != GetMuscles().end(); ++iter )
			{
				OpenSim::Muscle& osmus = dynamic_cast<Muscle_Simbody*>( iter->get() )->GetOsMuscle();
				auto a = override_activation != 0.0 ? override_activation : ( *iter )->GetControlValue();
				osmus.setActivation( GetOsimModel().updWorkingState(), a );
			}

			m_pOsimModel->equilibrateMuscles( GetTkState() );
		}

	}
}
