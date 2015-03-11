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
#include "boost/foreach.hpp"
#include "Leg_Simbody.h"
#include "../Factories.h"

#include <boost/thread.hpp>

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
		m_PrevIntStep( -1 )
		{
			String model_file;
			String state_init_file;

			INIT_FROM_PROP( props, integration_accuracy, 0.0001 );
			INIT_FROM_PROP( props, max_step_size, 0.001 );
			INIT_FROM_PROP_REQUIRED( props, model_file );
			INIT_FROM_PROP( props, state_init_file, String() );

			// create new OpenSim Model using resource cache
			m_pOsimModel = g_ModelCache.CreateCopy( model_file );

			// create the model
			CreateModelWrappers();

			// create controller dispatcher (ownership is automatically passed to OpenSim::Model)
			m_pControllerDispatcher = new ControllerDispatcher( *this );
			m_pOsimModel->addController( m_pControllerDispatcher );

			// Initialize the system
			// This is not thread-safe in case an exception is thrown, so we add a mutex guard
			g_SimBodyMutex.lock();
			m_pTkState = &m_pOsimModel->initSystem();
			g_SimBodyMutex.unlock();

			// Create the integrator for the simulation.
			m_pTkIntegrator = std::unique_ptr< SimTK::Integrator >( new SimTK::RungeKuttaMersonIntegrator( m_pOsimModel->getMultibodySystem() ) );
			m_pTkIntegrator->setAccuracy( integration_accuracy );
			m_pTkIntegrator->setMaximumStepSize( max_step_size );
			m_pTkIntegrator->resetAllStatistics();

			// read initial state
			if ( !state_init_file.empty() )
				ReadState( state_init_file );

			// create and initialize controllers
			const PropNode& cprops = props.GetChild( "Controllers" ).SetFlag();
			for ( auto iter = cprops.Begin(); iter != cprops.End(); ++iter )
				m_Controllers.push_back( CreateController( *iter->second, par, *this ) );

			// get initial controller values and equilibrate muscles
			for ( auto iter = GetControllers().begin(); iter != GetControllers().end(); ++iter )
				(*iter)->UpdateControls( *this, 0.0 );
			for ( auto iter = GetMuscles().begin(); iter != GetMuscles().end(); ++iter )
				dynamic_cast< Muscle_Simbody* >( iter->get() )->GetOsMuscle().setActivation( GetOsimModel().updWorkingState(), (*iter)->GetControlValue() );
			m_pOsimModel->equilibrateMuscles( GetOsimModel().updWorkingState() );

			// Create a manager to run the simulation. Can change manager options to save run time and memory or print more information
			m_pOsimManager = std::unique_ptr< OpenSim::Manager >( new OpenSim::Manager( *m_pOsimModel, *m_pTkIntegrator ) );
			m_pOsimManager->setWriteToStorage( true );
			m_pOsimManager->setPerformAnalyses( false );
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
				OpenSim::Actuator& osAct = m_pOsimModel->getActuators().get( idx );

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
			for ( int idx = 0; idx < m_pOsimModel->getBodySet().getSize(); ++idx )
				m_Bodies.push_back( BodyUP( new Body_Simbody( *this, m_pOsimModel->getBodySet().get( idx ) ) ) );

			// Create wrappers for joints
			m_Joints.clear();
			for ( int idx = 0; idx < m_pOsimModel->getJointSet().getSize(); ++idx )
				m_Joints.push_back( JointUP( new Joint_Simbody( m_pOsimModel->getJointSet().get( idx ) ) ) );

			// setup hierarchy and create wrappers
			m_RootLink = CreateLinkHierarchy( m_pOsimModel->getGroundBody() );

			// create legs
			const Link* left_femur = GetRootLink().FindLink( "femur_l" );
			if ( left_femur )
				m_Legs.push_back( LegUP( new Leg_Simbody( *this, *left_femur, left_femur->GetChild( 0 ).GetChild( 0 ), m_Legs.size(), LeftSide ) ) );
			const Link* right_femur = GetRootLink().FindLink( "femur_r" );
			if ( right_femur )
				m_Legs.push_back( LegUP( new Leg_Simbody( *this, *right_femur, right_femur->GetChild( 0 ).GetChild( 0 ), m_Legs.size(), RightSide ) ) );
		}

		void Model_Simbody::ReadState( const String& file )
		{
			// OpenSim: why is there no normal way to get a value using a label???

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

		void Model_Simbody::AdvanceSimulationTo( double time )
		{
			SCONE_ASSERT( m_pOsimManager );

			// Integrate from initial time to final time and integrate
			m_pOsimManager->setInitialTime( 0.0 );
			m_pOsimManager->setFinalTime( time );
			m_pOsimManager->integrate( GetTkState() );
		}

		void Model_Simbody::WriteStateHistory( const String& file )
		{
			m_pOsimManager->getStateStorage().print( file + ".sto" );
		}

		Vec3 Model_Simbody::GetComPos()
		{
			return ToVec3( m_pOsimModel->calcMassCenterPosition( GetTkState() ) );
		}
		
		Vec3 Model_Simbody::GetComVel()
		{
			return ToVec3( m_pOsimModel->calcMassCenterVelocity( GetTkState() ) );
		}

		Real Model_Simbody::GetMass()
		{
			return m_pOsimModel->getMultibodySystem().getMatterSubsystem().calcSystemMass( m_pOsimModel->getWorkingState() );
		}

		scone::Vec3 Model_Simbody::GetGravity()
		{
			return ToVec3( m_pOsimModel->getGravity() );
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
					link->GetChildren().push_back( CreateLinkHierarchy( childBody.m_osBody ) );
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
					m_Model.SetTerminationRequest();
			}

			// inject actuator values into controls
			SimTK::Vector controlValue( 1 );
			BOOST_FOREACH( MuscleUP& mus, m_Model.GetMuscles() )
			{
				controlValue[ 0 ] = mus->GetControlValue();
				dynamic_cast< Muscle_Simbody& >( *mus ).GetOsMuscle().addInControls( controlValue, controls );
			}

			// update previous control step
			m_Model.m_PrevIntStep = m_Model.GetIntegrationStep();
		}

		bool Model_Simbody::HasGroundContact()
		{
			// total vertical force applied to both feet
			m_pOsimModel->getMultibodySystem().realize( GetTkState(), SimTK::Stage::Dynamics );
			OpenSim::Array<double> force_foot_r = m_pOsimModel->getForceSet().get("foot_r").getRecordValues( GetTkState() );
			OpenSim::Array<double> force_foot_l = m_pOsimModel->getForceSet().get("foot_l").getRecordValues( GetTkState() );
			double netGRFVertical = force_foot_r[1] + force_foot_l[1];
			//printf("array_size=%d vertical force=%f\n", force_foot_l.size(), netGRFVertical );
			return netGRFVertical < -1.0;
		}

		void Model_Simbody::SetTerminationRequest()
		{
			Model::SetTerminationRequest();
			m_pOsimManager->halt();
		}

		double Model_Simbody::GetTime()
		{
			return GetTkState().getTime();
		}

		int Model_Simbody::GetIntegrationStep()
		{
			return GetTkIntegrator().getNumStepsTaken();
		}

		int Model_Simbody::GetPreviousIntegrationStep()
		{
			return m_PrevIntStep;
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

		scone::String Model_Simbody::GetSignature()
		{
			return GetOsimModel().getName();
		}
	}
}
