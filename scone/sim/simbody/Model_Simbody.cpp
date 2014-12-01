#include "stdafx.h"
#include "Model_Simbody.h"
#include "..\..\core\exceptions.h"
#include "Link_Simbody.h"
#include "Muscle_Simbody.h"
#include "World_Simbody.h"

namespace scone
{
	namespace sim
	{
		Model_Simbody::Model_Simbody( World_Simbody& world ) :
		m_World( world )
		{
		}

		Model_Simbody::~Model_Simbody()
		{
		}

		bool Model_Simbody::Load( const String& filename )
		{
			return false;
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
		
		size_t Model_Simbody::GetLinkCount()
		{
			SCONE_THROW_NOT_IMPLEMENTED;
		}
		
		Link_Simbody& Model_Simbody::GetLink( size_t idx )
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
}
}
