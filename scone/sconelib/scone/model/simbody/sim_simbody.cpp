#include "sim_simbody.h"
#include "Model_Simbody.h"
#include "scone/model/Factories.h"

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
