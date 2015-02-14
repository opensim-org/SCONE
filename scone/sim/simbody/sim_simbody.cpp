#include "stdafx.h"

#include "sim_simbody.h"
#include "../../core/PropNodeFactory.h"
#include "Model_Simbody.h"

namespace scone
{
	namespace sim
	{
		void RegisterSimbody()
		{
			Model_Simbody::RegisterFactory( "Simbody" );
		}
	}
}
