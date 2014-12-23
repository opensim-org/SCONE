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

		/// Constructor
		Model_Simbody::Model_Simbody( Simulation_Simbody& simulation, const String& filename ) :
		m_osModel( nullptr ),
		m_tkState( nullptr ),
		m_Simulation( simulation )
		{
			OpenSim::Object::setSerializeAllDefaults(true);
			m_osModel = std::unique_ptr< OpenSim::Model >( new OpenSim::Model( filename ) );

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
			SCONE_LOG( "Muscles created: " << m_Muscles.size() );

			// Create wrappers for bodies
			for ( int idx = 0; idx < m_osModel->getBodySet().getSize(); ++idx )
				m_Bodies.push_back( BodyUP( new Body_Simbody( m_osModel->getBodySet().get( idx ) ) ) );
			SCONE_LOG( "Bodies created: " << m_Bodies.size() );

			// Create wrappers for joints
			for ( int idx = 0; idx < m_osModel->getJointSet().getSize(); ++idx )
				m_Joints.push_back( JointUP( new Joint_Simbody( m_osModel->getJointSet().get( idx ) ) ) );
			SCONE_LOG( "Joints created: " << m_Joints.size() );

			// setup hierarchy and create wrappers
			m_RootLink = CreateLinkHierarchy( m_osModel->getGroundBody() );

			// debug print
			SCONE_LOG( m_RootLink->ToString() );
		}

		Model_Simbody::~Model_Simbody()
		{
		}

		void Model_Simbody::ProcessProperties( const PropNode& props )
		{
		}

		Vec3 Model_Simbody::GetComPos()
		{
			SimTK::Vec3 osVec = m_osModel->calcMassCenterPosition( GetTkState() );

			return ToVec3( osVec );
		}
		
		Vec3 Model_Simbody::GetComVel()
		{
			SimTK::Vec3 osVec = m_osModel->calcMassCenterVelocity( GetTkState() );
			
			return ToVec3( osVec );
		}

		Real Model_Simbody::GetMass()
		{
			return m_osModel->getMultibodySystem().getMatterSubsystem().calcSystemMass( m_osModel->getWorkingState() );
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
			// Initialize the system. initSystem() cannot be used here because adding the event handler
			// must be done between buildSystem() and initializeState().
			m_osModel->buildSystem();

			// TODO: add termination event handler
			//TerminateSimulation *terminate = new TerminateSimulation(osimModel, forceThreshold);
			//osimModel.updMultibodySystem().addEventHandler(terminate);

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
			m_tkIntegrator->setAccuracy( m_Simulation.integration_accuracy );

			// Create a manager to run the simulation. Can change manager options to save run time and memory or print more information
			OpenSim::Manager manager( *m_osModel, *m_tkIntegrator );
			manager.setWriteToStorage( true );
			manager.setPerformAnalyses( false );

			// Integrate from initial time to final time and integrate
			manager.setInitialTime( 0.0 );
			manager.setFinalTime( time );
			manager.integrate( GetTkState() );

			manager.getStateStorage().print("test_output.sto");
		}
	}
}
