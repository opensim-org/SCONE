#include "stdafx.h"

#include "../../core/Exception.h"
#include "../../core/Log.h"

#include "Model_Simbody.h"
#include "Body_Simbody.h"
#include "Muscle_Simbody.h"
#include "Simulation_Simbody.h"
#include "Joint_Simbody.h"
#include "tools.h"

#include <OpenSim/OpenSim.h>
#include "boost/foreach.hpp"

namespace scone
{
	namespace sim
	{
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

		/// Simbody event handler to determine termination
		class Model_Simbody::TerminationEventHandler : public SimTK::TriggeredEventHandler
		{
		public:
			TerminationEventHandler( Model_Simbody& model ) : m_Model( model ), SimTK::TriggeredEventHandler( SimTK::Stage::Dynamics ) { };
			//virtual Real getNextEventTime( const SimTK::State& state, bool includeCurrentTime ) const override { return state.getTime() + 0.001; }
			virtual Real getValue( const SimTK::State& s ) const override {
				double value = (double)m_Model.ShouldTerminate() - 0.5;
				//printf( "tt=%8.5f, steps=%d\n", s.getTime(), m_Model.GetTkIntegrator().getNumStepsTaken() );
				return value; }
			virtual void handleEvent( SimTK::State& state, Real accuracy, bool& shouldTerminate ) const override {
				shouldTerminate = m_Model.ShouldTerminate();
			}

		private:
			Model_Simbody& m_Model;
		};

		/// Constructor
		Model_Simbody::Model_Simbody( const PropNode& props ) :
		Model( props ),
		m_osModel( nullptr ),
		m_tkState( nullptr ),
		m_pControllerDispatcher( nullptr ),
		m_pTerminationEventHandler( nullptr )
		{
			INIT_FROM_PROP( props, integration_accuracy, 0.0001 );
			INIT_FROM_PROP( props, max_step_size, 0.001 );
			INIT_FROM_PROP( props, model_file, String("") );

			// create the model
			CreateModelFromFile( model_file );

			// create controllers
			InitFromPropNode( props.GetChild( "Controllers" ), m_Controllers );
		}

		Model_Simbody::~Model_Simbody()
		{
		}

		void Model_Simbody::CreateModelFromFile( const String& file )
		{
			m_osInitModel = std::unique_ptr< OpenSim::Model >( new OpenSim::Model( file ) );
			ResetModel();
		}

		void Model_Simbody::ResetModel()
		{
			SCONE_ASSERT( m_osInitModel );

			// explicitly reset existing objects
			m_osModel.reset(); 
			m_tkIntegrator.reset();
			m_osManager.reset();

			// create new osModel
			m_osModel = std::unique_ptr< OpenSim::Model >( new OpenSim::Model( *m_osInitModel ) );

			// Create wrappers for actuators
			m_Muscles.clear();
			for ( int idx = 0; idx < m_osModel->getActuators().getSize(); ++idx )
			{
				OpenSim::Actuator& osAct = m_osModel->getActuators().get( idx );

				try // see if it's a muscle
				{
					OpenSim::Muscle& osMus = dynamic_cast< OpenSim::Muscle& >( osAct );
					m_Muscles.push_back( MuscleUP( new Muscle_Simbody( osMus ) ) );
				}
				catch ( std::bad_cast& )
				{
					SCONE_THROW( "Unsupported actuator type" );
				}
			}

			// Create wrappers for bodies
			m_Bodies.clear();
			for ( int idx = 0; idx < m_osModel->getBodySet().getSize(); ++idx )
				m_Bodies.push_back( BodyUP( new Body_Simbody( *this, m_osModel->getBodySet().get( idx ) ) ) );

			// Create wrappers for joints
			m_Joints.clear();
			for ( int idx = 0; idx < m_osModel->getJointSet().getSize(); ++idx )
				m_Joints.push_back( JointUP( new Joint_Simbody( m_osModel->getJointSet().get( idx ) ) ) );

			// setup hierarchy and create wrappers
			m_RootLink = CreateLinkHierarchy( m_osModel->getGroundBody() );

			// create controller dispatcher (ownership is automatically passed to OpenSim::Model)
			m_pControllerDispatcher = new ControllerDispatcher( *this );
			m_osModel->addController( m_pControllerDispatcher );

			// reset termination flag
			m_ShouldTerminate = false;

			// Initialize the system
			m_tkState = &m_osModel->initSystem();

			// reset controllers
			BOOST_FOREACH( ControllerUP& c, m_Controllers )
			{
				c->SetTerminationRequest( false );
				c->Initialize( *this );
			}
		}

		void Model_Simbody::PrepareSimulation()
		{
			SCONE_ASSERT( m_osModel && m_tkState );

			// Create the integrator for the simulation.
			m_tkIntegrator = std::unique_ptr< SimTK::Integrator >( new SimTK::RungeKuttaMersonIntegrator( m_osModel->getMultibodySystem() ) );
			m_tkIntegrator->setAccuracy( integration_accuracy );
			m_tkIntegrator->setMaximumStepSize( max_step_size );
			m_tkIntegrator->resetAllStatistics();

			// get initial controller values and equilibrate muscles
			for ( auto iter = GetControllers().begin(); iter != GetControllers().end(); ++iter )
				(*iter)->UpdateControls( *this, 0.0 );
			for ( auto iter = GetMuscles().begin(); iter != GetMuscles().end(); ++iter )
				dynamic_cast< Muscle_Simbody* >( iter->get() )->GetOsMuscle().setActivation( GetOsModel().updWorkingState(), (*iter)->GetControlValue() );
			m_osModel->equilibrateMuscles( GetOsModel().updWorkingState() );

			// Create a manager to run the simulation. Can change manager options to save run time and memory or print more information
			m_osManager = std::unique_ptr< OpenSim::Manager >( new OpenSim::Manager( *m_osModel, *m_tkIntegrator ) );
			m_osManager->setWriteToStorage( true );
			m_osManager->setPerformAnalyses( false );
		}

		Vec3 Model_Simbody::GetComPos()
		{
			return ToVec3( m_osModel->calcMassCenterPosition( GetTkState() ) );
		}
		
		Vec3 Model_Simbody::GetComVel()
		{
			return ToVec3( m_osModel->calcMassCenterVelocity( GetTkState() ) );
		}

		Real Model_Simbody::GetMass()
		{
			return m_osModel->getMultibodySystem().getMatterSubsystem().calcSystemMass( m_osModel->getWorkingState() );
		}

		scone::Vec3 Model_Simbody::GetGravity()
		{
			return ToVec3( m_osModel->getGravity() );
		}

		bool is_body_equal( BodyUP& body, OpenSim::Body& osBody )
		{
			return dynamic_cast< Body_Simbody& >( *body ).m_osBody == osBody;
		}

		LinkUP Model_Simbody::CreateLinkHierarchy( OpenSim::Body& osBody )
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
				link = LinkUP( new Link( **itBody, **itJoint ) );
			}
			else
			{
				link = LinkUP( new Link( **itBody ) );
			}

			// add children
			for ( auto iter = m_Bodies.begin(); iter != m_Bodies.end(); ++iter )
			{
				Body_Simbody& childBody = dynamic_cast< Body_Simbody& >( **iter );
				if ( childBody.m_osBody.hasJoint() && childBody.m_osBody.getJoint().getParentBody() == osBody )
				{
					// create child link
					link->children().push_back( CreateLinkHierarchy( childBody.m_osBody ) );
				}
			}

			return link;
		}

		void Model_Simbody::ControllerDispatcher::computeControls( const SimTK::State& s, SimTK::Vector &controls ) const
		{
			// update current state (TODO: remove const cast)
			m_Model.SetTkState( const_cast< SimTK::State& >( s ) );

			// reset actuator values
			BOOST_FOREACH( MuscleUP& mus, m_Model.GetMuscles() )
				mus->ResetControlValue();

			// update all controllers
			BOOST_FOREACH( ControllerUP& con, m_Model.GetControllers() )
			{
				con->UpdateControls( m_Model, s.getTime() );
				if ( con->GetTerminationRequest() )
					m_Model.RequestTermination();
			}

			// inject actuator values into controls
			SimTK::Vector controlValue( 1 );
			BOOST_FOREACH( MuscleUP& mus, m_Model.GetMuscles() )
			{
				controlValue[ 0 ] = mus->GetControlValue();
				dynamic_cast< Muscle_Simbody& >( *mus ).GetOsMuscle().addInControls( controlValue, controls );
			}
		}

		void Model_Simbody::AdvanceSimulationTo( double time )
		{
			SCONE_ASSERT( m_osManager );

			// Integrate from initial time to final time and integrate
			m_osManager->setInitialTime( 0.0 );
			m_osManager->setFinalTime( time );
			m_osManager->integrate( GetTkState() );
		}

		void Model_Simbody::WriteStateHistory( const String& file )
		{
			m_osManager->getStateStorage().print( file + ".sto" );
		}

		void Model_Simbody::ProcessParameters( opt::ParamSet& par )
		{
			ResetModel();

			// attach controllers to model and process parameters
			BOOST_FOREACH( ControllerUP& c, m_Controllers )
			{
				c->Initialize( *this );
				c->ProcessParameters( par );
			}

			PrepareSimulation();
		}

		bool Model_Simbody::HasGroundContact()
		{
			// total vertical force applied to both feet
			m_osModel->getMultibodySystem().realize( GetTkState(), SimTK::Stage::Dynamics );
			OpenSim::Array<double> force_foot_r = m_osModel->getForceSet().get("foot_r").getRecordValues( GetTkState() );
			OpenSim::Array<double> force_foot_l = m_osModel->getForceSet().get("foot_l").getRecordValues( GetTkState() );
			double netGRFVertical = force_foot_r[1] + force_foot_l[1];
			//printf("array_size=%d vertical force=%f\n", force_foot_l.size(), netGRFVertical );
			return netGRFVertical < -1.0;
		}

		void Model_Simbody::RequestTermination()
		{
			Model::RequestTermination();
			m_osManager->halt();
		}

		double Model_Simbody::GetTime()
		{
			return GetTkState().getTime();
		}

		size_t Model_Simbody::GetStep()
		{
			return GetTkIntegrator().getNumStepsTaken();
		}
	}
}
