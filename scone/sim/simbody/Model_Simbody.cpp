#include "stdafx.h"

#include "../../core/Exception.h"
#include "../../core/Log.h"

#include "Model_Simbody.h"
#include "Body_Simbody.h"
#include "Muscle_Simbody.h"
#include "Simulation_Simbody.h"
#include "Joint_Simbody.h"
#include "tools.h"
#include "../../core/InitFromPropNode.h"

#include <OpenSim/OpenSim.h>
#include <OpenSim/Simulation/Model/Umberger2010MuscleMetabolicsProbe.h>
#include <OpenSim/Simulation/Model/Bhargava2004MuscleMetabolicsProbe.h>

#include "boost/foreach.hpp"
#include "../Factories.h"

#include <boost/thread.hpp>
#include <boost/filesystem.hpp>
#include "../../core/system.h"
#include "../../core/Profiler.h"
#include "Dof_Simbody.h"

using std::cout;
using std::endl;

#define USE_MANUAL_INTEGRATION 1

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
		m_pProbe( 0 )
		{
			SCONE_PROFILE_SCOPE;

			String model_file;
			String state_init_file;
			String probe_class;
			double pre_control_simulation_time;

			INIT_PROPERTY( props, integration_accuracy, 0.0001 );
			INIT_PROPERTY( props, integration_method, String( "RungeKuttaMerson" ) );
			INIT_PROPERTY( props, max_step_size, 0.001 );
			INIT_PROPERTY( props, use_fixed_control_step_size, false );
			INIT_PROPERTY( props, fixed_control_step_size, 0.001 );
			INIT_PROPERTY_REQUIRED( props, model_file );
			INIT_PROPERTY( props, state_init_file, String() );
			INIT_PROPERTY( props, probe_class, String() );
			INIT_PROPERTY( props, pre_control_simulation_time, 0.0 );

			// create new OpenSim Model using resource cache
			m_pOsimModel = g_ModelCache.CreateCopy( GetSconeFolder( "models" ) + model_file );

			// create the model
			CreateModelWrappers();

			// create controller dispatcher (ownership is automatically passed to OpenSim::Model)
			m_pControllerDispatcher = new ControllerDispatcher( *this );
			m_pOsimModel->addController( m_pControllerDispatcher );

			// create probe (ownership is automatically passed to OpenSim::Model)
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
				probe->setInitialConditions( SimTK::Vector( 1 ) );
				probe->setOperation("integrate");
				m_pProbe = probe;
			}

			// Initialize the system
			// This is not thread-safe in case an exception is thrown, so we add a mutex guard
			g_SimBodyMutex.lock();
			m_pTkState = &m_pOsimModel->initSystem();
			g_SimBodyMutex.unlock();

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
			else SCONE_THROW( "Invalid integration method: " + GetQuoted( integration_method ) );

			m_pTkIntegrator->setAccuracy( integration_accuracy );
			m_pTkIntegrator->setMaximumStepSize( max_step_size );
			m_pTkIntegrator->resetAllStatistics();

			// read initial state
			if ( !state_init_file.empty() )
				ReadState( GetSconeFolder( "models" ) + state_init_file );

			// Create a manager to run the simulation. Can change manager options to save run time and memory or print more information
			m_pOsimManager = std::unique_ptr< OpenSim::Manager >( new OpenSim::Manager( *m_pOsimModel, *m_pTkIntegrator ) );
			m_pOsimManager->setWriteToStorage( true );
			m_pOsimManager->setPerformAnalyses( false );
			m_pOsimManager->setInitialTime( 0.0 );

			// do some pre-control simulation
			if ( pre_control_simulation_time > 0.0 )
			{
				m_pOsimModel->equilibrateMuscles( GetTkState() );
				m_pOsimManager->setFinalTime( pre_control_simulation_time );
				m_pOsimManager->integrate( GetTkState() );
			}

			// TODO: perhaps realize velocity or dynamics here, so that controllers have access valid properties
			// right now, this is not needed because each individual call realizes the correct state

			// create and initialize controllers
			const PropNode& cprops = props.GetChild( "Controllers" ).Touch();
			for ( auto iter = cprops.Begin(); iter != cprops.End(); ++iter )
				m_Controllers.push_back( CreateController( *iter->second, par, *this, sim::Area::WHOLE_BODY ) );

			// update SensorDelayAdapters here because they may be needed by controllers
			// muscles are first equilibrated to ensure they contain valid data
			m_pOsimModel->equilibrateMuscles( GetTkState() );
			UpdateAnalyses();
			UpdateSensorDelayAdapters();

			// get initial controller values and inject results
			UpdateControlValues();
			for ( auto iter = GetMuscles().begin(); iter != GetMuscles().end(); ++iter )
				dynamic_cast< Muscle_Simbody* >( iter->get() )->GetOsMuscle().setActivation( GetOsimModel().updWorkingState(), (*iter)->GetControlValue() );

			// (re-)equilibrate muscles with initial control values set
			m_pOsimModel->equilibrateMuscles( GetTkState() );
		}

		Model_Simbody::~Model_Simbody()
		{
		}

		void Model_Simbody::CreateModelWrappers()
		{
			SCONE_ASSERT( m_pOsimModel );

			// Create wrappers for actuators
			m_Muscles.clear();
			for ( int idx = 0; idx < m_pOsimModel->getActuators().getSize(); ++idx )
			{
				// OpenSim: Set<T>::get( idx ) is const but returns non-const reference, is this a bug?
				OpenSim::Actuator& osAct = m_pOsimModel->getActuators().get( idx );

				try // see if it's a muscle
				{
					OpenSim::Muscle& osMus = dynamic_cast< OpenSim::Muscle& >( osAct );
					m_Muscles.push_back( MuscleUP( new Muscle_Simbody( *this, osMus ) ) );
					m_Sensors.push_back( m_Muscles.back().get() );
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
			m_Joints.clear();
			for ( int idx = 0; idx < m_pOsimModel->getJointSet().getSize(); ++idx )
				m_Joints.push_back( JointUP( new Joint_Simbody( *this, m_pOsimModel->getJointSet().get( idx ) ) ) );

			// create wrappers for dofs
			m_Dofs.clear();
			for ( int idx = 0; idx < m_pOsimModel->getCoordinateSet().getSize(); ++idx )
			{
				m_Dofs.push_back( DofUP( new Dof_Simbody( *this, m_pOsimModel->getCoordinateSet().get( idx ) ) ) );
				m_Sensors.push_back( m_Dofs.back().get() );
			}

			// setup hierarchy and create wrappers
			m_RootLink = CreateLinkHierarchy( m_pOsimModel->getGroundBody() );

			// create legs and connect contact forces
			if ( Link* left_femur = m_RootLink->FindLink( "femur_l" ) )
			{
				Link& left_foot = left_femur->GetChild( 0 ).GetChild( 0 );
				m_Legs.push_back( LegUP( new Leg( *left_femur, left_foot, m_Legs.size(), LeftSide ) ) );
				dynamic_cast< Body_Simbody& >( left_foot.GetBody() ).ConnectContactForce( "foot_l" );
			}

			if ( Link* right_femur = m_RootLink->FindLink( "femur_r" ) )
			{
				Link& right_foot = right_femur->GetChild( 0 ).GetChild( 0 );
				m_Legs.push_back( LegUP( new Leg( *right_femur, right_femur->GetChild( 0 ).GetChild( 0 ), m_Legs.size(), RightSide ) ) );
				dynamic_cast< Body_Simbody& >( right_foot.GetBody() ).ConnectContactForce( "foot_r" );
			}
		}

		void Model_Simbody::ReadState( const String& file )
		{
			// OSIM: why is there no normal way to get a value using a label???

			// create a copy of the storage
			auto store = g_StorageCache.CreateCopy( file );
			OpenSim::Array< double > data = store->getStateVector( 0 )->getData();
			OpenSim::Array< std::string > storeLabels = store->getColumnLabels();
			OpenSim::Array< std::string > stateNames = GetOsimModel().getStateVariableNames();

			// run over all labels
			for ( int i = 0; i < storeLabels.getSize(); i++ )
			{
				// check if the label is corresponds to a state
				if ( stateNames.findIndex( storeLabels[ i ] ) != -1 )
				{
					double value = data[ store->getStateIndex( storeLabels[i] ) ];
					GetOsimModel().setStateVariable( GetTkState(), storeLabels[i], value );
				}
				//else SCONE_LOG( "Unused state parameter: " << storeLabels[ i ] );
			}
		}

		String Model_Simbody::WriteStateHistory( const String& file ) const
		{
			boost::filesystem::path path( file + ".sto" );
			m_pOsimManager->getStateStorage().setName( ( path.parent_path().filename() / path.stem() ).string() );
			m_pOsimManager->getStateStorage().print( path.string() );
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

		Real Model_Simbody::GetMass() const
		{
			return m_pOsimModel->getMultibodySystem().getMatterSubsystem().calcSystemMass( m_pOsimModel->getWorkingState() );
		}

		scone::Vec3 Model_Simbody::GetGravity() const
		{
			return ToVec3( m_pOsimModel->getGravity() );
		}

		bool is_body_equal( BodyUP& body, OpenSim::Body& osBody )
		{
			return dynamic_cast< Body_Simbody& >( *body ).m_osBody == osBody;
		}

		scone::sim::LinkUP Model_Simbody::CreateLinkHierarchy( OpenSim::Body& osBody, Link* parent )
		{
			LinkUP link;

			// find the sim::Body
			auto itBody = std::find_if( m_Bodies.begin(), m_Bodies.end(), [&]( BodyUP& body ){ return dynamic_cast< Body_Simbody& >( *body ).m_osBody == osBody; } );
			SCONE_ASSERT( itBody != m_Bodies.end() );

			// find the sim::Joint (if any)
			if ( osBody.hasJoint() )
			{
				auto itJoint = std::find_if( m_Joints.begin(), m_Joints.end(), [&]( JointUP& body ){ return dynamic_cast< Joint_Simbody& >( *body ).m_osJoint == osBody.getJoint(); } );
				SCONE_ASSERT( itJoint != m_Joints.end() );
				link = LinkUP( new Link( **itBody, **itJoint, parent ) );
			}
			else
			{
				// this is the root Link
				link = LinkUP( new Link( **itBody ) );
			}

			// add children
			for ( auto iter = m_Bodies.begin(); iter != m_Bodies.end(); ++iter )
			{
				Body_Simbody& childBody = dynamic_cast< Body_Simbody& >( **iter );
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
			//log::TraceF( "%03d %03d %.8f", m_Model.GetIntegrationStep(), m_Model.GetPreviousIntegrationStep(), s.getTime() );

			// see 'catch' statement below for explanation try {} catch {} is needed
			try
			{
				if ( !m_Model.use_fixed_control_step_size )
				{
					// update current state (TODO: remove const cast)
					m_Model.SetTkState( const_cast< SimTK::State& >( s ) );

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
					BOOST_FOREACH( MuscleUP& mus, m_Model.GetMuscles() )
					{
						// This is an optimization that only works when there are only muscles
						// OpenSim: addInControls is rather inefficient, that's why we changed it
						// TODO: fix this into a generic version
						controls[ idx++ ] += mus->GetControlValue();
						//controlValue[ 0 ] = mus->GetControlValue();
						//dynamic_cast< Muscle_Simbody& >( *mus ).GetOsMuscle().addInControls( controlValue, controls );
					}
				}
			}
			catch( std::exception& e )
			{
				// exceptions are caught and reported here
				// otherwise they get lost in SimTK::AbstractIntegratorRep::attemptDAEStep()
				// OpenSim: please remove the catch(...) statement
				log::Critical( e.what() );
				throw e;
			}
		}

		void Model_Simbody::AdvanceSimulationTo( double final_time )
		{
			SCONE_PROFILE_SCOPE;
			SCONE_ASSERT( m_pOsimManager );

			if ( use_fixed_control_step_size )
			{
				// set this because it's used by GetSimulationEndTime()
				// TODO: Change this!
				m_pOsimManager->setFinalTime( final_time );

				// Integrate using time stepper
				m_pTkIntegrator->setFinalTime( final_time );
				SimTK::TimeStepper ts( m_pOsimModel->getMultibodySystem(), *m_pTkIntegrator );
				ts.initialize( GetTkState() );
				int number_of_steps = static_cast< int >( 0.5 + final_time / fixed_control_step_size );
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

					// OpenSim: add state to storage, why so complicated?
					{
						SCONE_PROFILE_SCOPE_NAMED( "m_pOsimManager->getStateStorage()::append()" );
						OpenSim::Array<double> stateValues;
						m_pOsimModel->getStateValues( GetTkState(), stateValues );
						OpenSim::StateVector vec;
						vec.setStates( GetTkState().getTime(), stateValues.getSize(), &stateValues[0]);
						m_pOsimManager->getStateStorage().append(vec);					}

					// update the sensor delays and other analyses
					UpdateSensorDelayAdapters();
					UpdateAnalyses();

					// terminate on request
					if ( GetTerminationRequest() || status == SimTK::Integrator::EndOfSimulation )
					{
						log::DebugF( "Terminating simulation at %.6f", ts.getTime() );
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
			SCONE_ASSERT( m_pProbe != nullptr );
			return m_pProbe->getProbeOutputs( GetTkState() )[ 0 ];
		}

		std::map< String, double > Model_Simbody::GetState() const
		{
			auto values = GetOsimModel().getStateValues( GetTkState() );
			auto names = GetOsimModel().getStateVariableNames();
			SCONE_ASSERT( values.size() == names.size() );

			std::map< String, double > state;
			for ( int i = 0; i < values.size(); ++i )
				state[ names[ i ] ] = values[ i ];

			return state;
		}

		double Model_Simbody::GetSimulationEndTime() const
		{
			return m_pOsimManager->getFinalTime();
		}

		scone::String Model_Simbody::GetMainSignature() const
		{
			return GetOsimModel().getName();
		}

		const String& Model_Simbody::GetName() const 
		{
			return GetOsimModel().getName();
		}
	}
}
