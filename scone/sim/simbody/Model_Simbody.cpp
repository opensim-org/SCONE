#include "stdafx.h"
#include "Model_Simbody.h"
#include "..\..\core\Exception.h"
#include "Body_Simbody.h"
#include "Muscle_Simbody.h"
#include "Simulation_Simbody.h"

#include <OpenSim/OpenSim.h>

namespace scone
{
	namespace sim
	{
		/// Adapter class for Simbody controller
		class Model_Simbody::ControllerAdapter : public OpenSim::Controller
		{
		public:
			ControllerAdapter( ControllerSP controller ) : m_pController( controller ) { };
			virtual void computeControls( const SimTK::State& s, SimTK::Vector &controls ) const override {
				m_pController->Update( s.getTime() );
			}

			virtual Controller* clone() const override {
				return new ControllerAdapter( m_pController );
			}

			virtual const std::string& getConcreteClassName() const override {
				throw std::logic_error("The method or operation is not implemented.");
			}

		private:
			ControllerSP m_pController;
		};

		Model_Simbody::Model_Simbody( Simulation_Simbody* simulation ) :
		m_pSimulation( simulation ),
		m_pModel( nullptr )
		{
		}

		Model_Simbody::~Model_Simbody()
		{
		}

		bool Model_Simbody::Load( const String& filename )
		{
			m_pModel = std::unique_ptr< OpenSim::Model >( new OpenSim::Model( filename ) );
			return true;
		}

		Vec3 Model_Simbody::GetComPos()
		{
			SCONE_THROW_NOT_IMPLEMENTED;
		}
		
		Vec3 Model_Simbody::GetComVel()
		{
			SCONE_THROW_NOT_IMPLEMENTED;
		}
		
		Real Model_Simbody::GetMass()
		{
			SCONE_THROW_NOT_IMPLEMENTED;
		}
		
		size_t Model_Simbody::GetBodyCount()
		{
			SCONE_THROW_NOT_IMPLEMENTED;
		}
		
		Body_Simbody& Model_Simbody::GetBody( size_t idx )
		{
			SCONE_THROW_NOT_IMPLEMENTED;
		}
		
		size_t Model_Simbody::GetMuscleCount()
		{
			SCONE_THROW_NOT_IMPLEMENTED;
		}
		
		Muscle_Simbody& Model_Simbody::GetMuscle( size_t idx )
		{
			SCONE_THROW_NOT_IMPLEMENTED;
		}

		void Model_Simbody::AddController( ControllerSP controller )
		{
			m_Controllers.push_back( std::unique_ptr< ControllerAdapter >( new ControllerAdapter( controller ) ) );
		}
	}
}
