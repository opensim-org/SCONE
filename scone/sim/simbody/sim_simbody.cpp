#include "sim_simbody.h"
#include "Model_Simbody.h"
#include "scone/sim/Factories.h"

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
