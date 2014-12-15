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
		Model_Simbody::Model_Simbody( Simulation_Simbody* simulation ) :
		m_pSimulation( simulation )
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

		}
	}
}
