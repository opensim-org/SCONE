#include "stdafx.h"

#include "..\..\core\Exception.h"

#include "Model_Simbody.h"
#include "Body_Simbody.h"
#include "Muscle_Simbody.h"
#include "Simulation_Simbody.h"
#include <OpenSim/OpenSim.h>
#include "Joint_Simbody.h"

namespace scone
{
	namespace sim
	{
		/// Simbody controller that calls scone controllers
		class Model_Simbody::ControllerDispatcher : public OpenSim::Controller
		{
		public:
			ControllerDispatcher( Model_Simbody& model ) : m_Model( model ) { };
			virtual void computeControls( const SimTK::State& s, SimTK::Vector &controls ) const override
			{
				// reset actuator values
				std::vector< ActuatorUP >& actvec = m_Model.GetActuators();
				for ( auto iter = actvec.begin(); iter != actvec.end(); ++iter )
					(*iter)->ResetControlValue();

				// update all controllers
				for ( auto iter = m_Model.GetControllers().begin(); iter != m_Model.GetControllers().end(); ++iter )
					(*iter)->Update( s.getTime() );

				// inject actuator values into controls
				SCONE_ASSERT( getActuatorSet().getSize() == actvec.size() );
				SimTK::Vector controlValue( 1 );
				for ( size_t idx = 0; idx < actvec.size(); ++idx )
				{
					controlValue[ 0 ] = actvec[ idx ]->GetControlValue();
					m_Model.GetOpenSimModel().getActuators().get( idx ).addInControls( controlValue, controls );
				}
			}

			virtual ControllerDispatcher* clone() const override {
				return new ControllerDispatcher( *this );
			}

			virtual const std::string& getConcreteClassName() const override {
				throw std::logic_error("The method or operation is not implemented.");
			}

		private:
			Model_Simbody& m_Model;
		};

		Model_Simbody::Model_Simbody() :
		m_osModel( nullptr )
		{
		}

		Model_Simbody::~Model_Simbody()
		{
		}

		bool Model_Simbody::Load( const String& filename )
		{
			m_osModel = std::unique_ptr< OpenSim::Model >( new OpenSim::Model( filename ) );

			// Create wrappers for actuators
			for ( int idx = 0; idx < m_osModel->getActuators().getSize(); ++idx )
			{
				OpenSim::Actuator& osAct = m_osModel->getActuators().get( idx );

				try // see if it's a muscle
				{
					OpenSim::Muscle& osMus = dynamic_cast< OpenSim::Muscle& >( osAct );
					m_Actuators.push_back( MuscleUP( new Muscle_Simbody( osMus ) ) );
					m_Muscles.push_back( MuscleUP( new Muscle_Simbody( osMus ) ) );
				}
				catch ( std::bad_cast& )
				{
					SCONE_THROW( "Unsupported actuator type" );
				}
			}

			// Create wrappers for bodies
			for ( int idx = 0; idx < m_osModel->getBodySet().getSize(); ++idx )
				m_Bodies.push_back( BodyUP( new Body_Simbody( m_osModel->getBodySet().get( idx ) ) ) );

			// Create wrappers for joints
			for ( int idx = 0; idx < m_osModel->getJointSet().getSize(); ++idx )
				m_Joints.push_back( JointUP( new Joint_Simbody( m_osModel->getJointSet().get( idx ) ) ) );

			return true;
		}

		Vec3 Model_Simbody::GetComPos()
		{
			SimTK::Vec3 osVec = m_osModel->getMultibodySystem().getMatterSubsystem().calcSystemMassCenterLocationInGround( m_osModel->getWorkingState() );

			return Vec3( osVec[0], osVec[1], osVec[2] ); // TODO: faster initialization?
		}
		
		Vec3 Model_Simbody::GetComVel()
		{
			SimTK::Vec3 osVec = m_osModel->getMultibodySystem().getMatterSubsystem().calcSystemMassCenterVelocityInGround( m_osModel->getWorkingState() );
			
			return Vec3( osVec[0], osVec[1], osVec[2] ); // TODO: faster initialization?
		}

		Real Model_Simbody::GetMass()
		{
			return m_osModel->getMultibodySystem().getMatterSubsystem().calcSystemMass( m_osModel->getWorkingState() );
		}
	}
}
