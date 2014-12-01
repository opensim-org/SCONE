#include "stdafx.h"
#include "Driver_Simbody.h"

#include "World_Simbody.h"

namespace scone
{
	namespace sim
	{
		std::unique_ptr< World > Driver_Simbody::CreateWorld()
		{
			return std::unique_ptr< World >( new World_Simbody );
		}
	}
}
