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
			virtual Real getValue( const SimTK::State& ) const override {
				double value = (double)m_Model.ShouldTerminate() - 0.5;
				//printf("bla%f ", value );
				return value; }
			virtual void handleEvent( SimTK::State& state, Real accuracy, bool& shouldTerminate ) const override
			{
				if ( m_Model.ShouldTerminate() )
					printf("termination at time %f\n", state.getTime() );
				shouldTerminate = m_Model.ShouldTerminate();
			}

		private:
			Model_Simbody& m_Model;
		};

		/// Constructor
		Model_Simbody::Model_Simbody( const String& filename ) :
		m_osModel( nullptr ),
		m_tkState( nullptr ),
		m_pControllerDispatcher( nullptr ),
		m_pTerminationEventHandler( nullptr )
		{
		}

		Model_Simbody::~Model_Simbody()
		{
		}

		void Model_Simbody::CreateOsModel( const String& file )
		{
			m_osModel = std::unique_ptr< OpenSim::Model >( new OpenSim::Model( file ) );

			// Create wrappers for actuators
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
			//SCONE_LOG( "Muscles created: " << m_Muscles.size() );

			// Create wrappers for bodies
			for ( int idx = 0; idx < m_osModel->getBodySet().getSize(); ++idx )
				m_Bodies.push_back( BodyUP( new Body_Simbody( m_osModel->getBodySet().get( idx ) ) ) );
			//SCONE_LOG( "Bodies created: " << m_Bodies.size() );

			// Create wrappers for joints
			for ( int idx = 0; idx < m_osModel->getJointSet().getSize(); ++idx )
				m_Joints.push_back( JointUP( new Joint_Simbody( m_osModel->getJointSet().get( idx ) ) ) );
			//SCONE_LOG( "Joints created: " << m_Joints.size() );

			// setup hierarchy and create wrappers
			m_RootLink = CreateLinkHierarchy( m_osModel->getGroundBody() );

			// debug print
			//SCONE_LOG( m_RootLink->ToString() );

			// create controller dispatcher (ownership is automatically passed to OpenSim::Model)
			m_pControllerDispatcher = new ControllerDispatcher( *this );
			m_osModel->addController( m_pControllerDispatcher );
		}

		void Model_Simbody::ProcessProperties( const PropNode& props )
		{
			Model::ProcessProperties( props );

			INIT_FROM_PROP( props, integration_accuracy, 0.0001 );
			INIT_FROM_PROP( props, model_file, String("") );
			
			if ( !model_file.empty() )
				CreateOsModel( model_file );

			InitControllers();
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
			// reset actuator values
			BOOST_FOREACH( MuscleUP& mus, m_Model.GetMuscles() )
				mus->ResetControlValue();

			// update all controllers
			BOOST_FOREACH( ControllerUP& con, m_Model.GetControllers() )
				con->UpdateControls( m_Model, s.getTime() );

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
			// reset termination flag
			m_ShouldTerminate = false;

			// Initialize the system. initSystem() cannot be used here because adding the event handler
			// must be done between buildSystem() and initializeState().
			m_osModel->buildSystem();

			// create termination event handler (TODO: verify ownership is passed)
			m_pTerminationEventHandler = new TerminationEventHandler( *this );
			m_osModel->updMultibodySystem().addEventHandler( m_pTerminationEventHandler );

			// create model state and keep pointer (non-owning)
			m_tkState = &m_osModel->initializeState();

			// initial call to all controllers
			for ( auto iter = GetControllers().begin(); iter != GetControllers().end(); ++iter )
				(*iter)->UpdateControls( *this, 0.0 );

			// update initial muscle activations and equilibrate
			for ( auto iter = GetMuscles().begin(); iter != GetMuscles().end(); ++iter )
				dynamic_cast< Muscle_Simbody* >( iter->get() )->GetOsMuscle().setActivation( GetTkState(), (*iter)->GetControlValue() );
			m_osModel->equilibrateMuscles( GetTkState() );

			// Create the integrator for the simulation.
			m_tkIntegrator = std::unique_ptr< SimTK::Integrator >( new SimTK::RungeKuttaMersonIntegrator( m_osModel->getMultibodySystem() ) );
			m_tkIntegrator->setAccuracy( integration_accuracy );

			// Create a manager to run the simulation. Can change manager options to save run time and memory or print more information
			m_osManager = std::unique_ptr< OpenSim::Manager >( new OpenSim::Manager( *m_osModel, *m_tkIntegrator ) );
			m_osManager->setWriteToStorage( true );
			m_osManager->setPerformAnalyses( false );
			
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
			Model::ProcessParameters( par );
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
	}
}
