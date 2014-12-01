#include "stdafx.h"
#include "World_Simbody.h"
#include "..\..\core\exceptions.h"

namespace scone
{
	namespace sim
	{
		std::unique_ptr< Model > scone::sim::World_Simbody::CreateModel(String& filename)
		{
			SCONE_THROW_NOT_IMPLEMENTED;
		}
	}
}
