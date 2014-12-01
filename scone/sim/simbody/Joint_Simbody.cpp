#include "stdafx.h"
#include "Joint_Simbody.h"
#include "..\..\core\exceptions.h"


namespace scone
{
	namespace sim
	{
		class Link& scone::sim::Joint_Simbody::GetParentLink()
		{
			SCONE_THROW_NOT_IMPLEMENTED;
		}
		
		class Link& scone::sim::Joint_Simbody::GetChildLink()
		{
			SCONE_THROW_NOT_IMPLEMENTED;
		}
		
		class Model& scone::sim::Joint_Simbody::GetModel()
		{
			SCONE_THROW_NOT_IMPLEMENTED;
		}
	}
}
