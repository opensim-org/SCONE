#include "stdafx.h"

#include "sim_simbody.h"
#include "../../core/PropNodeFactory.h"
#include "Model_Simbody.h"
#include "../Factories.h"

namespace scone
{
	namespace sim
	{
		void RegisterSimbody()
		{
			GetModelFactory().Register< sim::Model_Simbody >( "Simbody" );
		}
	}
}
