#include "stdafx.h"
#include "Joint_Simbody.h"
#include "..\..\core\Exception.h"


namespace scone
{
	namespace sim
	{
		class Body& scone::sim::Joint_Simbody::GetParentLink()
		{
			SCONE_THROW_NOT_IMPLEMENTED;
		}
		
		class Body& scone::sim::Joint_Simbody::GetChildLink()
		{
			SCONE_THROW_NOT_IMPLEMENTED;
		}
		
		class Model& scone::sim::Joint_Simbody::GetModel()
		{
			SCONE_THROW_NOT_IMPLEMENTED;
		}
	}
}
